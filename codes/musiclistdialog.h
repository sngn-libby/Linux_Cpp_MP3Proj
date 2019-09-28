#ifndef MUSICLISTDIALOG_H
#define MUSICLISTDIALOG_H

#include <QDialog>
#include "musicdialog.h"
#include <qlistwidget.h>
#include <qicon.h>

namespace Ui {
class MusicListDialog;
}

class MusicListDialog : public QDialog
{
    Q_OBJECT
    
//public slots:
//    QListWidgetItem
public:
    explicit MusicListDialog(QWidget *parent = 0);
    ~MusicListDialog();
    
private slots:
    void on_list_title_itemClicked(QListWidgetItem *item);
    //MusicListDialog *musiclistDialog;

    void on_btn_add_song_clicked();

private:
    Ui::MusicListDialog *ui;
    int fd_pipe[2];
    QStringList IconPath, ListContext, MusicList;
};

#endif // MUSICLISTDIALOG_H
