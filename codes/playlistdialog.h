#ifndef PLAYLISTDIALOG_H
#define PLAYLISTDIALOG_H

#include <QDialog>
#include <qtablewidget.h>
#include <musicdialog.h>


namespace Ui {
class PlayListDialog;
}

class PlayListDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit PlayListDialog(QWidget *parent = 0);
    ~PlayListDialog();
    
private slots:
    void on_btn_add_song_clicked();

    void on_playlist_table_activated(const QModelIndex &index);

    void on_playlist_table_itemDoubleClicked(QTableWidgetItem *item);

    void on_btn_remove_song_clicked();

    void on_playlist_table_itemClicked(QTableWidgetItem *item);

    void on_group_cover_clicked();

private:
    Ui::PlayListDialog *ui;

    MusicDialog *musicDialog;

signals:
    void playCommand(const QString file_path);

};

#endif // PLAYLISTDIALOG_H
