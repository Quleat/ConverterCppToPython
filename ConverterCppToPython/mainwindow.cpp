#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "converterctop.h"
#include <sstream>
#include <QDebug>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    QString text = ui->textEdit->toPlainText();
    string str = text.toStdString();
    Converter conv;
    stringstream stream = conv.TranslateOuter(str);
    QString output(stream.str().c_str());
    ui->textEdit_2->setText(output);
}
