#ifndef WIDGET_HPP
#define WIDGET_HPP

#include <kernel/kernel.hpp>

#include <QImage>
#include <QDirIterator>
#include <QDebug>
#include <QDateTime>
#include <QScreen>
#include <QFileDialog>
#include <QMessageBox>
#include <QListWidget>

#include <stdio.h>

namespace Ui {
class Widget;
}

struct ImageData
{
    cv::Mat hash;
    QString filename;
    ImageData(const cv::Mat &hash, const QString &filename);
};

QString format_file_size(qlonglong bytes);

QImage get_image_icon(const QString &image_name);

size_t get_files_cnt(std::unique_ptr<QDirIterator> dir_it);

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

signals:
    void progress_state_changed(double current, double total);

private slots:
    void on_browse_clicked();
    void on_scan_clicked();
    void on_remove_clicked();
    void update_info();
    void update_progress_bar(double current, double total);
    void on_location_textChanged();

private:
    std::vector<std::unique_ptr<ImageData>> get_hashes_pool();
    std::vector<std::vector<std::unique_ptr<ImageData>>> get_similarity_clusters(
            std::vector<std::unique_ptr<ImageData>> &images_pool);
    std::vector<std::vector<std::unique_ptr<ImageData>>> get_similarity_clusters(
            std::vector<std::unique_ptr<ImageData>> &&images_pool);
    void build_similarities_list(
            const std::vector<std::vector<std::unique_ptr<ImageData>>> &similarity_clusters);
    void build_similarities_list(
            std::vector<std::vector<std::unique_ptr<ImageData>>> &&similarity_clusters);
    void resize_relative_to_screen_size(double width_multiplier, double height_multiplier);
    void remove_adjucent_blank_items();
    QImage get_current_item_thumbnail() const;
    QString get_current_item_info() const;
    void insert_blank_item();

    Ui::Widget *ui;
    PHashHandler hash_handler;
};

void redirect_stderr_to_log();

#endif // WIDGET_HPP
