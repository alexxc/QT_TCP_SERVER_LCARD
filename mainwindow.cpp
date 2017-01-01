#include "mainwindow.h"
#include "ui_mainwindow.h"




MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    e502=new E502();
}

MainWindow::~MainWindow()
{
    delete ui;

}

void MainWindow::on_starting_clicked()
{
    e502->start();
}

void MainWindow::on_stoping_clicked()
{
    e502->stop();
}



