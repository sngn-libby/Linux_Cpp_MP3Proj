#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <iostream>
#include <QFileDialog>
#include <string.h>
#include <QSlider>
#include "musicdialog.h"
#include "ui_musicdialog.h"
#include "playlistdialog.h"


#define MAX (2000)
const char *ans_meta = "ANS_META_";
char rbuf[MAX];
char music_len[100], cur_time[100];
struct INFO { char title[100]; char artist[50]; char released[20]; char len[20]; };
INFO music[100];


extern QStringList playlist;
extern volatile int idx_playing;
extern volatile int idx_max;

pid_t pid_playing;
volatile int play_flag, pause_flag, interrupt_flag, continue_flag;
volatile int vol;

MusicDialog::MusicDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MusicDialog)
{
    ui->setupUi(this);

    stopped = false;
    play_flag = 0;
    pause_flag = 0;
    interrupt_flag = 0;
    if(pipe(fd_pipe) == -1) close();
    if(pipe(fd_pipe_info) == -1) close();

    musicThread = new MusicThread();
    connect(musicThread, SIGNAL(sendCommand(int)), this, SLOT(handleCommand(int)));
    connect(musicThread, SIGNAL(startnextCommand(int)), this, SLOT(handoverCommand(int)));
    musicThread->start();

    vol = ui->volumSlider->value();

    //playString = QString("mplayer -slave -quiet -vf screenshot -wid 1120 -zoom -xy 400 tv:// -tv driver=v4l2:device=/dev/video0");

    //mplayer -input cmdlist;
    // get_file_name, get_meta_album, get_meta_artist, get_meta_genre, get_meta_title, get_time_length, get_time_pos, loadlist,
}

MusicDialog::~MusicDialog()
{
    delete ui;
}

/* signal & slot */
void MusicDialog::handleCommand(int cmd)
{
    if(cmd == 1 && play_flag) // while playing music
    {
        /* MP3 Playing Effect */
        ui->_music_title->setStyleSheet("color:rgb(0, 255, 0)");
        ui->_music_title->setStyleSheet("color:rgb(0, 0, 0)");
    }
    else if(cmd == 1 && !play_flag)
    {
        ui->_music_title->setStyleSheet("color:rgb(0, 0, 0)");
        ui->mplayBTN->setStyleSheet("background-image: url(:/images/qdemo/images/play.png);");
    }
}

void MusicDialog::handoverCommand(int)
{
    if(++idx_playing >= idx_max) idx_playing = 0;
    this->on_mplayBTN_clicked();
}

void MusicDialog::receiveCommand(const QString file_path) // thread main function
{
    if(!file_path.isEmpty())
    {
        ui->_opendescrip->setText(file_path);
        QStringList music_title = file_path.split("/"); // show music_title only (strip directories)
        music_title = music_title[4].split(".");
        QString album_dir = QString("background-image: url(:/images/qdemo/images/%1.png);").arg(music_title[0]);
        ui->album_cover->setStyleSheet(album_dir);

        this->on_mplayBTN_clicked();
    }
}

/* main button */
void MusicDialog::on__closeBTN_clicked()
{
    if(play_flag)
    {
        interrupt_flag = 1;
        write(fd_pipe[1], "quit\n", 5);
        play_flag = 0;
        pause_flag = 0;
    }
    ::close(fd_pipe[0]);
    ::close(fd_pipe[1]);
    close();
}

/* MP3 Player + realted button */
void sigChldHandler(int signal)
{
    printf("got signal %d\n", signal);

    play_flag = 0;
    pause_flag = 0;

    if(interrupt_flag)
    {
        interrupt_flag = 0;
        return;
    }

    continue_flag = 1;
}

void MusicDialog::on_mplayBTN_clicked()
{
    pid_t pid_temp;
    char cmd[256];

    if(play_flag)
    {
        write(fd_pipe[1], "pause\n", 6);
        pause_flag ^= 1;

        if(pause_flag)
        {
            ui->mplayBTN->setStyleSheet("background-image: url(:/images/qdemo/images/play.png);");
        }
        else
        {
            ui->mplayBTN->setStyleSheet("background-image: url(:/images/qdemo/images/pause.png);");
        }

        return;
    }

    play_flag = 1; pause_flag = 0;
    ui->mplayBTN->setStyleSheet("background-image: url(:/images/qdemo/images/pause.png);");
    pid_temp = fork();
    if(pid_temp == -1) exit(-1);
    else if(pid_temp == 0)
    {
        char tmp[10];

        ::close(0);
        dup(fd_pipe[0]); // read -> stdin
        ::close(fd_pipe[0]);
        ::close(fd_pipe[1]);
        strcpy(cmd, ui->_opendescrip->text().toStdString().c_str());

        ::close(1);
        dup(fd_pipe_info[1]); // write -> stdout
        ::close(fd_pipe_info[0]);
        ::close(fd_pipe_info[1]);

        sprintf(tmp, "%d", vol);
        execlp("/mnt/nfs/mplayer", "mplayer", "-slave", "-quiet", "-volume", tmp, "-srate", "44100", cmd, NULL);
    }
    else
    {
        char *pos;
        char *ret;
        const char *trash = "'";

        pid_playing = pid_temp;
        sighandler_t sig_ret;
        sig_ret = signal(SIGCHLD, sigChldHandler);
        if(sig_ret == SIG_ERR) exit(-1);

        /* read music info from console */
        usleep(700000);
        write(fd_pipe[1], "get_meta_title\n", 15);
        usleep(500000);
        read(fd_pipe_info[0], rbuf, MAX-1);
        pos = strstr(rbuf, ans_meta);
        strchr(pos, '\n')[0] = 0;

        strcpy(music[idx_playing].title, &pos[16]);
        ret = strstr(music[idx_playing].title, trash);
        strchr(ret, '\'')[0] = 0;
        ui->_music_title->setText(&music[idx_playing].title[0]);

        write(fd_pipe[1], "get_meta_artist\n", 16);
        usleep(500000);
        read(fd_pipe_info[0], rbuf, MAX-1);
        pos = strstr(rbuf, ans_meta);
        strchr(pos, '\n')[0] = 0;

        strcpy(music[idx_playing].artist, &pos[17]);
        ret = strstr(music[idx_playing].artist, trash);
        strchr(ret, '\'')[0] = 0;
        ui->_artist->setText(&music[idx_playing].artist[0]);

        write(fd_pipe[1], "get_meta_year\n", 14);
        usleep(500000);
        read(fd_pipe_info[0], rbuf, MAX-1);
        pos = strstr(rbuf, ans_meta);
        strchr(pos, '\n')[0] = 0;

        strcpy(music[idx_playing].released, &pos[14]);
        ret = strstr(music[idx_playing].released, trash);
        strchr(ret, '\'')[0] = 0;

//        write(fd_pipe[1], "get_time_length\n", 16);
//        usleep(700000);
//        read(fd_pipe_info[0], &rbuf[0], MAX-1);
//        pos = strstr(rbuf, ans_meta);
//        strchr(pos, '\n')[0] = 0;
//        printf("music length : %s\n", &pos[0]);
//        strcpy(music[idx_playing].len, &pos[16]);
//        ret = strstr(music[idx_playing].len, trash);
//        strchr(ret, '\'')[0] = 0;

    }
}

/* MP3 Playing Buttons : prev, next */
void MusicDialog::on_mplayBTN_prev_clicked()
{
    if(play_flag)
    {
        interrupt_flag = 1;
        write(fd_pipe[1], "quit\n", 5);
        while(play_flag);
    }
    pause_flag = 0;

    if(--idx_playing <= 0) idx_playing=idx_max-1;

    QString file_path = playlist[idx_playing];
    if(!file_path.isEmpty())
    {
        ui->_opendescrip->setText(file_path);
        QStringList music_title = file_path.split("/"); // show music_title only (strip directories)
        music_title = music_title[4].split(".");
        QString album_dir = QString("background-image: url(:/images/qdemo/images/%1.png);").arg(music_title[0]);
        ui->album_cover->setStyleSheet(album_dir);
    }

    this->on_mplayBTN_clicked();
}

void MusicDialog::on_mplayBTN_next_clicked()
{

    if(play_flag)
    {
        interrupt_flag = 1;
        //printf("next btn : %d %d\n", fd_pipe[1], fd_pipe_info[0]);
        write(fd_pipe[1], "quit\n", 5);
        while(play_flag);

    }
    pause_flag = 0;

    if(++idx_playing >= idx_max) idx_playing=0;

    QString file_path = playlist[idx_playing];
    if(!file_path.isEmpty())
    {
        ui->_opendescrip->setText(file_path);
        QStringList music_title = file_path.split("/"); // show music_title only (strip directories)
        music_title = music_title[4].split(".");
        QString album_dir = QString("background-image: url(:/images/qdemo/images/%1.png);").arg(music_title[0]);
        ui->album_cover->setStyleSheet(album_dir);
    }

    this->on_mplayBTN_clicked();
}


void MusicDialog::on_volumSlider_valueChanged(int value)
{
    char cmd[12];

    if(value > 100 || value < 0) return;

    //printf("volume : %d\n", value);
    vol = value;
    sprintf(cmd, "volume %02d 1\n", value);
    //printf("volume btn : %d %d\n", fd_pipe[1], fd_pipe_info[0]);
    write(fd_pipe[1], cmd, 12);
}


void MusicDialog::on_album_cover_clicked()
{
    //write(fd_pipe[1], "get_meta_title\n", 15);
}

