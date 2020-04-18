#include "widget.hpp"
#include "ui_widget.h"

struct ImageData
{
    cv::Mat hash;
    QString filename;
    ImageData(const cv::Mat &hash, const QString &filename);
};

ImageData::ImageData(const cv::Mat &hash, const QString &filename) :
    hash(hash), filename(filename) {}

static QString format_file_size(qlonglong bytes)
{
    QString b = QString("%L1").arg(bytes, -1, 'f', 0, ' ') + " bytes";
    double kb = static_cast<double>(bytes) / 1000;
    if (kb < 1.)
        return b;
    double mb = kb / 1000;
    if (mb < 1.)
        return QString::number(kb, 'f', 1) + " kB (" + b + ")";
    return QString::number(mb, 'f', 1) + " MB (" + b + ")";
}

static QImage get_image_icon(const QString &image_name)
{
    return QImage(image_name).scaled(QSize(32, 32), Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

static size_t get_files_cnt(std::unique_ptr<QDirIterator> dir_it)
{
    size_t files_cnt = 0;
    while (dir_it->hasNext())
    {
        dir_it->next();
        ++files_cnt;
    }
    return files_cnt;
}

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),
    hash_handler([](double hashes_diff) -> bool
    {
        return hashes_diff <= 5;
    }),
    progress_dialog(nullptr)
{
    ui->setupUi(this);
    resize_relative_to_screen_size(0.8, 0.8);
    connect(ui->browse, &QPushButton::clicked,
            this, &Widget::slot_browse_clicked);
    connect(ui->scan, &QPushButton::clicked,
            this, &Widget::slot_scan_clicked);
    connect(ui->remove, &QPushButton::clicked,
            this, &Widget::slot_remove_clicked);
    connect(ui->list, &QListWidget::currentItemChanged,
            this, &Widget::slot_list_currentItemChanged);
    connect(ui->location, &QLineEdit::textChanged,
            this, &Widget::slot_location_textChanged);
    connect(this, &Widget::signal_progress_state_changed,
            this, &Widget::slot_progress_state_changed);
    connect(this, &Widget::signal_progress_text_changed,
            this, &Widget::slot_progress_text_changed);
    connect(this, &Widget::signal_progress_closed,
            this, &Widget::slot_progress_closed);
    connect(this, &Widget::signal_item_added,
            this, &Widget::slot_item_added);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::slot_browse_clicked()
{
    QString directory = QFileDialog::getExistingDirectory(this, "Open Directory",
                                                          ui->location->text(),
                                                          QFileDialog::ShowDirsOnly |
                                                          QFileDialog::DontResolveSymlinks);
    if (!directory.isEmpty())
        ui->location->setText(directory);
}

void Widget::slot_scan_clicked()
{
    ui->list->clear();
    ui->image->clear();
    ui->info->clear();
    progress_dialog = new QProgressDialog(this);
    progress_dialog->setWindowModality(Qt::WindowModal);
    progress_dialog->setCancelButton(nullptr);
    progress_dialog->setAttribute(Qt::WA_DeleteOnClose);
    progress_dialog->show();
    std::thread([this]()
    {
        build_similarities_list(get_similarity_clusters(get_hashes_pool()));
    }).detach();
}

void Widget::slot_remove_clicked()
{
    std::vector<QListWidgetItem *> items_to_remove;
    for (int i = 0; i < ui->list->count(); ++i)
    {
        QListWidgetItem *item = ui->list->item(i);
        if ((item->flags() & Qt::ItemIsEnabled) == 0)
            continue;
        if (item->checkState())
            items_to_remove.push_back(item);
    }
    if (items_to_remove.empty())
        return;
    if (QMessageBox::question(this, "Attention", QString::number(items_to_remove.size()) +
                              " checked images will be permanently removed.\n"
                              "Are you sure you want to proceed?",
                              QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
        return;
    for (auto item : items_to_remove)
    {
        QFile(item->text()).remove();
        qDebug() << "Removed" << item->text();
        delete item;
    }
    remove_adjucent_blank_items();
}

void Widget::slot_list_currentItemChanged(QListWidgetItem *, QListWidgetItem *)
{
    if (ui->list->currentRow() == -1)
        return;
    ui->image->setPixmap(QPixmap::fromImage(get_current_item_thumbnail()));
    ui->image->setAlignment(Qt::AlignCenter);
    ui->info->setAlignment(Qt::AlignTop);
    ui->info->setText(get_current_item_info());
}

void Widget::slot_location_textChanged()
{
    ui->scan->setEnabled(!ui->location->text().isEmpty() && QDir(ui->location->text()).exists());
}

void Widget::slot_progress_state_changed(double current, double total)
{
    progress_dialog->setValue(current / total * 100);
}

void Widget::slot_progress_text_changed(const QString &text)
{
    progress_dialog->setLabelText(text);
    progress_dialog->setValue(0);
}

void Widget::slot_progress_closed()
{
    progress_dialog->close();
}

void Widget::slot_item_added(QListWidgetItem *item)
{
    ui->list->insertItem(0, item);
}

HashesPool Widget::get_hashes_pool()
{
    emit signal_progress_text_changed("Building hashes pool (stage 1 of 3)...");
    auto init_dir_it =
            [path = ui->location->text()]() -> std::unique_ptr<QDirIterator>
    {
        return std::make_unique<QDirIterator>(path,
                                              QStringList() << "*.jpg" << "*.jpeg" << "*.png"
                                              << "*.tiff" << "*.tif", QDir::Files,
                                              QDirIterator::Subdirectories);
    };
    size_t files_cnt = get_files_cnt(init_dir_it());
    std::unique_ptr<QDirIterator> dir_it = init_dir_it();
    HashesPool hashes_pool;
    for (size_t files_scanned = 0; dir_it->hasNext(); ++files_scanned)
    {
        emit signal_progress_state_changed(files_scanned + 1, files_cnt);
        dir_it->next();
        cv::Mat img;
        try
        {
            img = cv::imread(dir_it->filePath().toStdString());
            if (img.empty())
                throw std::runtime_error("Empty image " + dir_it->filePath().toStdString());
        }
        catch(const std::runtime_error &e)
        {
            qDebug() << e.what();
            continue;
        }
        hashes_pool.push_back(std::make_unique<ImageData>(hash_handler.compute(img),
                                                          dir_it->filePath()));
    }
    return hashes_pool;
}

std::vector<SimilarityCluster> Widget::get_similarity_clusters(HashesPool &&hashes_pool)
{
    emit signal_progress_text_changed("Building similarity clusters (stage 2 of 3)...");
    std::vector<SimilarityCluster> similarity_clusters;
    for (size_t i = 0; i < hashes_pool.size(); ++i)
    {
        emit signal_progress_state_changed(i + 1, hashes_pool.size());
        if (hashes_pool.at(i) == nullptr)
            continue;
        SimilarityCluster similarity_cluster;
        for (size_t j = i + 1; j < hashes_pool.size(); ++j)
        {
            if (hashes_pool.at(j) == nullptr)
                continue;
            if (hash_handler.compare(hashes_pool.at(i)->hash, hashes_pool.at(j)->hash))
                similarity_cluster.push_back(std::move(hashes_pool.at(j)));
        }
        if (!similarity_cluster.empty())
        {
            similarity_cluster.push_back(std::move(hashes_pool.at(i)));
            similarity_clusters.push_back(std::move(similarity_cluster));
        }
    }
    return similarity_clusters;
}

void Widget::build_similarities_list(const std::vector<SimilarityCluster> &similarity_clusters)
{
    emit signal_progress_text_changed("Building similarities list (stage 3 of 3)...");
    for (size_t i = 0; i < similarity_clusters.size(); ++i)
    {
        emit signal_progress_state_changed(i + 1, similarity_clusters.size());
        insert_blank_item();
        for (const auto &image_data : similarity_clusters.at(i))
        {
            QListWidgetItem *item = new QListWidgetItem;
            item->setIcon(QIcon(QPixmap::fromImage(get_image_icon(image_data->filename))));
            item->setText(image_data->filename);
            item->setCheckState(Qt::Unchecked);
            emit slot_item_added(item);
        }
    }
    emit signal_progress_closed();
}

void Widget::resize_relative_to_screen_size(double width_multiplier,
                                            double height_multiplier)
{
    QSize screen_size = qApp->screens().at(0)->size();
    resize(screen_size.width() * width_multiplier, screen_size.height() * height_multiplier);
}

void Widget::remove_adjucent_blank_items()
{
    for (int i = 1; i < ui->list->count(); ++i)
        if (ui->list->item(i - 1)->text().isEmpty() && ui->list->item(i)->text().isEmpty())
        {
            delete ui->list->item(i - 1);
            --i;
        }
}

QImage Widget::get_current_item_thumbnail() const
{
    return QImage(ui->list->currentItem()->text()).scaled(ui->image->size(), Qt::KeepAspectRatio,
                                                          Qt::SmoothTransformation);
}

QString Widget::get_current_item_info() const
{
    QFileInfo file_info(ui->list->currentItem()->text());
    QString info_string = "File path: " + file_info.absoluteFilePath() + "\n" +
            "Size: " + format_file_size(file_info.size()) + "\n" +
            "Created: " + file_info.created().toString() + "\n" +
            "Last modified: " + file_info.lastModified().toString();
    return info_string;
}

void Widget::insert_blank_item()
{
    QListWidgetItem *blank_item = new QListWidgetItem;
    blank_item->setFlags(Qt::NoItemFlags);
    ui->list->insertItem(0, blank_item);
}
