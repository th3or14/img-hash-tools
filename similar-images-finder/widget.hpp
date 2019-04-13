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

struct ImageData;

typedef std::vector<std::unique_ptr<ImageData>> HashesPool, SimilarityCluster;

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
    void on_list_currentItemChanged(QListWidgetItem *, QListWidgetItem *);
    void on_progress_state_changed(double current, double total);
    void on_location_textChanged();

private:
    HashesPool get_hashes_pool();
    std::vector<SimilarityCluster> get_similarity_clusters(HashesPool &hashes_pool);
    std::vector<SimilarityCluster> get_similarity_clusters(HashesPool &&hashes_pool);
    void build_similarities_list(const std::vector<SimilarityCluster> &similarity_clusters);
    void build_similarities_list(std::vector<SimilarityCluster> &&similarity_clusters);
    void resize_relative_to_screen_size(double width_multiplier, double height_multiplier);
    void remove_adjucent_blank_items();
    QImage get_current_item_thumbnail() const;
    QString get_current_item_info() const;
    void insert_blank_item();

    Ui::Widget *ui;
    PHashHandler hash_handler;
};

#endif // WIDGET_HPP
