#ifndef MUSICTHREAD_H
#define MUSICTHREAD_H

#include <QThread>

class MusicThread : public QThread
{
    Q_OBJECT
public:
    explicit MusicThread(QObject *parent = 0);
    void run();
    void stop();

private:
    volatile bool stopped;
    
signals:
    void sendCommand(int);
    void startnextCommand(int);
    
public slots:
    
};

#endif // MUSICTHREAD_H
