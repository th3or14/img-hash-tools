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
    SimilarImagesFinder();
    ~SimilarImagesFinder();

signals:
    void signal_scan_stage_iteration_completed(double, double);
    void signal_scan_stage_started(const QString &);
    void signal_scan_finished();
    void signal_item_added(QListWidgetItem *);

private slots:
    void slot_browse_clicked();
    void slot_scan_started();
    void slot_remove_clicked();
    void slot_list_current_item_changed(QListWidgetItem *, QListWidgetItem *);
    void slot_location_text_changed();
    void slot_scan_stage_iteration_completed(double current, double total);
    void slot_scan_stage_started(const QString &text);
    void slot_scan_finished();
    void slot_item_added(QListWidgetItem *item);

private:
    HashesPool get_hashes_pool();
    std::vector<SimilarityCluster> get_similarity_clusters(HashesPool &&hashes_pool);
    void build_similarities_list(const std::vector<SimilarityCluster> &similarity_clusters);
    void resize_relatively_to_screen_size(double width_multiplier, double height_multiplier);
    void remove_adjucent_blank_items();
    QImage get_current_item_thumbnail() const;
    QString get_current_item_info() const;
    void init_progress_dialog();
    void deinit_progress_dialog();
    void clear_ui();
    void setup_connections();

    Ui::Widget *ui;
    HashHandler hash_handler;
    QProgressDialog *progress_dialog;
};

#endif // SIMILAR_IMAGES_FINDER_HPP
