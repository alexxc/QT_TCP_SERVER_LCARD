#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <e502.h>




namespace Ui {
    class MainWindow;
}

class QTcpServer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_starting_clicked();
    void on_stoping_clicked();


private:
    Ui::MainWindow *ui;
    E502 *e502;
};

#endif // MAINWINDOW_H
