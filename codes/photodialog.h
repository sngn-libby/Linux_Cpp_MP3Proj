#ifndef PHOTODIALOG_H
#define PHOTODIALOG_H

#include <QDialog>

namespace Ui {
class PhotoDialog;
}

class PhotoDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit PhotoDialog(QWidget *parent = 0);
    ~PhotoDialog();
    
private slots:
    void on__closeBTN_clicked();

    void on_pcapture_BTN_clicked();

private:
    Ui::PhotoDialog *ui;
    volatile bool stopped;

    int fd_pipe_stdin[2];
    int fd_pipe_stdout[2];
    int capture_count;
};

#endif // PHOTODIALOG_H
