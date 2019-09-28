#include <stdio.h>
#include <signal.h>
#include "musiclistdialog.h"
#include "ui_musiclistdialog.h"
#include "musicdialog.h"
#include <QFileDialog>

volatile int play_flag;

MusicListDialog::MusicListDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MusicListDialog)
{
    ui->setupUi(this);

    play_flag = 0;
    //if(pipe(fd_pipe) == -1) close();

    /* Add myList to List Widget */
//    for(int i=0; i<IconPath.count(); i++)
//    {
//        QListWidgetItem *item = new QListWidgetItem(QIcon(IconPath.at(i)));
//        ui->list_title->addItem(item);
//    }
    //ui->list_title->addItems(":/images/qdemo/images/test.png"); // QStringList, Labels
    //ui->list_singer->addItems();
    //ui->list_info->addItems();
    //musiclistThread = new MusicListThread();
    //connect(musicThread, SIGNAL(sendCommand(int)), this, SLOT(handleCommand(int)));
    //musicThread->start();
}

MusicListDialog::~MusicListDialog()
{
    delete ui;
}
// album_photo

//void MusicListDialog::on_list_title_itemClicked(QListWidgetItem *item)
//{
//    pid_t pid_temp;
//    char cmd[256];

//    /* change COLOUR of selected item */
//    item->listWidget()->currentItem()->setBackgroundColor(Qt::blue);
//    item->listWidget()->currentItem()->setForeground(Qt::white);

//    if(play_flag)
//    {
//        write(fd_pipe[1], "p", 1);
//        return;
//    }
//    play_flag = 1;
//    pid_temp = fork();
//    if(pid_temp == -1) exit(-1);
//    else if(pid_temp == 0)
//    {
//        ::close(0);
//        dup(fd_pipe[0]);
//        ::close(fd_pipe[0]);
//        ::close(fd_pipe[1]);

//        MusicDialog dlg;
//        dlg.exec();
//        strcpy(cmd, item->text().toStdString().c_str());
//        execlp("/mnt/nfs/mplayer", "mplayer", "-volume", "30", "-srate", "44100", cmd, NULL);
//    }
//    else
//    {
//        sighandler_t sig_ret;
//        sig_ret = signal(SIGCHLD, sigChldHandler);
//        if(sig_ret == SIG_ERR) exit(-1);
//    }

//    /*-----------------------------------*/

////    QString fileName = QFileDialog::getOpenFileName(\
////                this, tr("Open Music"), "/mnt/nfs/test_contents", tr("Music Files (*.mp3 *.wav)"));
////    QString full_dir = fileName;
////    QStringList music_title = full_dir.split("/"); // show music_title only (strip directories)

////    if(!fileName.isEmpty())
////    {
////        ui->_opendescrip->setText(fileName);
////        music_title = music_title[4].split(".");
////        QString album_dir = QString("background-image: url(:/images/qdemo/images/%1.png);").arg(music_title[0]);
////        ui->album_photo->setStyleSheet(album_dir);
////    }

////    MusicDialog dlg;
////    dlg.exec();
//}

void MusicListDialog::on_btn_add_song_clicked()
{
    int cnt=0;

    QString fileName = QFileDialog::getOpenFileName(\
                this, tr("Open Music"), "/mnt/nfs/test_contents", tr("Music Files (*.mp3 *.wav)"));
    QStringList music_title = fileName.split("/"); // show music_title only (strip directories)
    music_title = music_title[4].split(".");

    ListContext<<QString("%1").arg(++cnt);
    IconPath<<fileName;
    MusicList<<music_title[0];
    ui->list_title->addItem("data is inserted.");
    //ui->listWidget_client1->scrollToBottom();
}
