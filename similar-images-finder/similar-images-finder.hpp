#ifndef SIMILAR_IMAGES_FINDER_HPP
#define SIMILAR_IMAGES_FINDER_HPP

#include <hash-handler/hash-handler.hpp>

#include <QImage>
#include <QDirIterator>
#include <QDebug>
#include <QDateTime>
#include <QScreen>
#include <QFileDialog>
#include <QMessageBox>
#include <QListWidget>
#include <QProgressDialog>

#include <thread>

namespace Ui {
class Widget;
}

struct ImageData;

typedef std::vector<std::unique_ptr<ImageData>> HashesPool, SimilarityCluster;

class SimilarImagesFinder : public QWidget
{
    Q_OBJECT

public:
    explicit SimilarImagesFinder(QWidget *parent = 0);
    ~SimilarImagesFinder();

signals:
    void signal_progress_state_changed(double current, double total);
    void signal_progress_text_changed(const QString &new_format);
    void signal_progress_closed();
    void signal_item_added(QListWidgetItem *item);

private slots:
    void slot_browse_clicked();
    void slot_scan_clicked();
    void slot_remove_clicked();
    void slot_list_currentItemChanged(QListWidgetItem *, QListWidgetItem *);
    void slot_location_textChanged();
    void slot_progress_state_changed(double current, double total);
    void slot_progress_text_changed(const QString &text);
    void slot_progress_closed();
    void slot_item_added(QListWidgetItem *item);

private:
    HashesPool get_hashes_pool();
    std::vector<SimilarityCluster> get_similarity_clusters(HashesPool &&hashes_pool);
    void build_similarities_list(const std::vector<SimilarityCluster> &similarity_clusters);
    void resize_relatively_to_screen_size(double width_multiplier, double height_multiplier);
    void remove_adjucent_blank_items();
    QImage get_current_item_thumbnail() const;
    QString get_current_item_info() const;
    void insert_blank_item();

    Ui::Widget *ui;
    HashHandler hash_handler;
    QProgressDialog *progress_dialog;
};

#endif // SIMILAR_IMAGES_FINDER_HPP
