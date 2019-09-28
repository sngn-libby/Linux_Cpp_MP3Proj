#include "photodialog.h"
#include "ui_photodialog.h"
#include <stdio.h>

PhotoDialog::PhotoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PhotoDialog)
{
    ui->setupUi(this);


    pid_t pid_temp;
    capture_count = 0;

    if(pipe(fd_pipe_stdin) == -1) exit(-1);
    if(pipe(fd_pipe_stdout) == -1) exit(-1);

    pid_temp = fork();
    if(pid_temp == -1) exit(-1);
    else if(pid_temp == 0)
    {
        ::close(0);
        dup(fd_pipe_stdin[0]);
        ::close(fd_pipe_stdin[0]);
        ::close(fd_pipe_stdin[1]);
        ::close(1);
        dup(fd_pipe_stdout[1]);
        ::close(fd_pipe_stdout[0]);
        ::close(fd_pipe_stdout[1]);
        // execute v4l2test
        execlp("./v4l2test", "v4l2test", "-d", "/dev/video5", "-c", "700000", "-b",\
               "/dev/fb0", "-x", "8", "-y", "40", "-a", "1", NULL);
    }
    else // close unnecessary file descriptor
    {
        ::close(fd_pipe_stdin[0]);
        ::close(fd_pipe_stdout[1]);
    }
}

PhotoDialog::~PhotoDialog()
{
    delete ui;
}

void PhotoDialog::on__closeBTN_clicked()
{
    write(fd_pipe_stdin[1], "e\n", 2);
    sleep(1);
    close();
}

void PhotoDialog::on_pcapture_BTN_clicked()
{
    char fname[128];
    char str[128];
    char cmd[256];
    int xpos[4] = {656, 824, 656, 824};
    int ypos[4] = {40, 40, 168, 168};

    printf("Capture %d\n", capture_count);
    sprintf(fname, "./cam/CAM%04d", capture_count);
    sprintf(str, "c ./cam/CAM%04d\n", capture_count);
    printf("send: %s", str);
    fflush(stdout);

    write(fd_pipe_stdin[1], str, strlen(str));
    read(fd_pipe_stdout[0], str, 64);
    str[2] = '\0';
    printf("recv: %s\n", str);
    fflush(stdout);
    sprintf(cmd, "./v4l2test -d /dev/video5 -c 70000 -b /dev/fb0 -x %d -y %d -a 3 -n %s",\
            xpos[capture_count%4], ypos[capture_count%4], fname);
    system(cmd);
    capture_count++;
}
