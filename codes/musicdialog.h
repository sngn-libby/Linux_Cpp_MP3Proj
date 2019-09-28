#ifndef MUSICDIALOG_H
#define MUSICDIALOG_H

#include <musicthread.h>
#include <QDialog>

namespace Ui {
class MusicDialog;
}

class MusicDialog : public QDialog
{
    Q_OBJECT
    
public slots:
    void handleCommand(int cmd);
    void handoverCommand(int);
    void receiveCommand(const QString file_path);

public:
    explicit MusicDialog(QWidget *parent = 0);
    ~MusicDialog();
    void run();
    void stop();
    
private slots:
    void on__closeBTN_clicked();

    void on_mplayBTN_clicked();

    void on_mplayBTN_prev_clicked();

    void on_mplayBTN_next_clicked();

    void on_album_cover_clicked();

    void on_volumSlider_valueChanged(int value);

private:
    Ui::MusicDialog *ui;
    int fd_pipe[2];
    int fd_pipe_info[2];

    MusicThread *musicThread;
    volatile bool stopped;

signals:
    //void sendCommand(const QString file_path);
};

#endif // MUSICDIALOG_H
