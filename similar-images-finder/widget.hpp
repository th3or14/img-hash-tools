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
#include <QProgressBar>
#include <QProgressDialog>

#include <stdio.h>

namespace Ui {
class Widget;
}

struct ImageData;

typedef std::vector<std::unique_ptr<ImageData>> HashesPool, SimilarityCluster;

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

signals:
    void signal_progress_bar_state_changed(double current, double total);
    void signal_progress_bar_format_changed(const QString &new_format);

private slots:
    void slot_browse_clicked();
    void slot_scan_clicked();
    void slot_remove_clicked();
    void slot_list_currentItemChanged(QListWidgetItem *, QListWidgetItem *);
    void slot_location_textChanged();
    void slot_progress_bar_state_changed(double current, double total);
    void slot_progress_bar_format_changed(const QString &new_format);

private:
    HashesPool get_hashes_pool();
    std::vector<SimilarityCluster> get_similarity_clusters(HashesPool &&hashes_pool);
    void build_similarities_list(const std::vector<SimilarityCluster> &similarity_clusters);
    void resize_relative_to_screen_size(double width_multiplier, double height_multiplier);
    void remove_adjucent_blank_items();
    QImage get_current_item_thumbnail() const;
    QString get_current_item_info() const;
    void insert_blank_item();

    Ui::Widget *ui;
    PHashHandler hash_handler;
    QProgressBar *progress_bar;
    QProgressDialog *progress_dialog;
};

#endif // WIDGET_HPP
