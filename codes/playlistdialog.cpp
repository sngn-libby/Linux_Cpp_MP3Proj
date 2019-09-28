#include <stdio.h>
#include <signal.h>
#include <QFileDialog>
#include "playlistdialog.h"
#include "ui_playlistdialog.h"

QStringList playlist = (QStringList());
QStringList playlist_title = (QStringList());
QStringList playlist_singer = (QStringList());
QStringList playlist_date = (QStringList());
volatile int idx_playing, idx_max;
struct INFO { char title[100]; char artist[50]; char released[20]; char len[20]; };
extern struct INFO music[100];


PlayListDialog::PlayListDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlayListDialog)
{

    ui->setupUi(this);
    ui->group_background->setStyleSheet("background-image: url(:/images/qdemo/images/album_background.png);");
    ui->group_cover->setStyleSheet("background-image: url(:/images/qdemo/images/group_cover.png);");
    ui->label->setStyleSheet("background-color:rgba(0, 0, 0, 100)");

    ui->playlist_table->setStyleSheet("QHeaderView::section { background-color:white }");

    ui->playlist_table->setColumnWidth(1, 140);
    ui->playlist_table->setColumnWidth(2, 130);
    ui->playlist_table->setColumnWidth(3, 140);
    ui->playlist_table->setColumnWidth(4, 90);

    for(int i=0; i<playlist_title.count(); i++)

    {
        ui->playlist_table->insertRow(i);

        QTableWidgetItem *item_title = new QTableWidgetItem(playlist_title[i]);
        QTableWidgetItem *item_path = new QTableWidgetItem(playlist[i]);
        QTableWidgetItem *item_artist = new QTableWidgetItem(music[i].artist);
        QTableWidgetItem *item_released = new QTableWidgetItem(music[i].released);
        ui->playlist_table->setItem(i, 0, item_title);
        ui->playlist_table->setItem(i, 1, item_artist);
        ui->playlist_table->setItem(i, 2, item_released);
        ui->playlist_table->setItem(i, 3, item_path);

        //ui->playlist_table->addItem(playlist_title[i]);
        //ui->ListWidget_Option->addItem(new QListWidgetItem(QIcon(IconPath.at(i)),ListContext.at(i)));


        /* related icon */
//        QTableWidgetItem *cubesHeaderItem = new QTableWidgetItem(tr("Cubes"));
//        cubesHeaderItem->setIcon(QIcon(QPixmap(":/Images/cubed.png")));
//        cubesHeaderItem->setTextAlignment(Qt::AlignVCenter);
    }



}

PlayListDialog::~PlayListDialog()
{
    delete ui;
}


void PlayListDialog::on_playlist_table_activated(const QModelIndex &index)
{

    ui->playlist_table->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
}


void PlayListDialog::on_btn_add_song_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(\
                this, tr("Open Music"), "/mnt/nfs/test_contents", tr("Music Files (*.mp3 *.wav)"));

    if(!fileName.isEmpty())
    {
        QStringList music_title = fileName.split("/"); // show music_title only (strip directories)
        music_title = music_title[4].split(".");
        QString album_dir = QString("background-image: url(:/images/qdemo/images/%1.png);").arg(music_title[0]);
        //ui->group_cover->setStyleSheet(album_dir);

        idx_max = ui->playlist_table->rowCount();
        ui->playlist_table->setRowCount(idx_max);


        playlist << fileName;
        /* need to add singer, date */
        //QListWidgetItem *item_cover = new QListWidgetItem(QIcon(album_dir));

        ui->playlist_table->insertRow(idx_max);
        int xbegin = fileName.lastIndexOf("/")+1;
        int xend = fileName.lastIndexOf('.') - xbegin ;
        QString title = fileName.mid(xbegin, xend);
        //playlist_title << title;

        QTableWidgetItem *item_title = new QTableWidgetItem(title);
        QTableWidgetItem *item_path = new QTableWidgetItem(fileName);

        ui->playlist_table->setItem(idx_max, 0, item_title);
        ui->playlist_table->setItem(idx_max, 3, item_path);
        idx_max++;
        printf("idx_max --> %d\n", idx_max);
    }
}

void PlayListDialog::on_playlist_table_itemClicked(QTableWidgetItem *item)
{
    QTableWidgetItem selected_item = item->row();
}

void PlayListDialog::on_btn_remove_song_clicked()
{
    QItemSelectionModel *selected = ui->playlist_table->selectionModel();

    if(selected->hasSelection())
    {
        int m_current_row = selected->currentIndex().row();
        ui->playlist_table->removeRow(m_current_row);
    }
}

/* MP3 Playing */

void PlayListDialog::on_playlist_table_itemDoubleClicked(QTableWidgetItem *item)
{
    item->setBackgroundColor(0x7896);
    idx_playing = item->row();

    /* make playing window, using thread concepts */
    musicDialog = new MusicDialog();
    connect(this, SIGNAL(playCommand(const QString)), musicDialog, SLOT(receiveCommand(const QString)));
    emit playCommand(playlist[idx_playing]);
    musicDialog->setGeometry(624, 0, 400, 600);
    musicDialog->exec();
}

void PlayListDialog::on_group_cover_clicked()
{
    close();
}
