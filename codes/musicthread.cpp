#include <stdio.h>
#include "musicthread.h"

extern int continue_flag;

MusicThread::MusicThread(QObject *parent) :
    QThread(parent)
{
    stopped = false; // initiate stoped variable
}

void MusicThread::run()
{
    while(stopped == false)
    {
        msleep(5);
        emit sendCommand(1);

        if(continue_flag)
        {
            continue_flag = 0;
            emit startnextCommand(1);
        }
    }
}

void MusicThread::stop()
{
    stopped = true;
    printf("song over --> stop sign/n");
    emit startnextCommand(1);
}
