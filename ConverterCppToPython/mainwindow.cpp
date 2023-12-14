#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "converterctop.h"
#include <sstream>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QFileDialog>
#include <QSaveFile>

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
    if(text.isEmpty()){
        QMessageBox::information(0, "info", "The field is empty\nThere's nothing to translate.");
        return;
    }

    string str = text.toStdString();
    Converter conv;
    try{
        stringstream stream = conv.TranslateOuter(str);
        QString output(stream.str().c_str());
        ui->textEdit_2->setText(output);
    }catch (exception &e){
        auto str = e.what();
        string errorMessage = str;
        QString qStr = QString::fromStdString(errorMessage);
        QMessageBox::critical(0, "error", "The error has occured while translating the code:\n" + qStr);
    }
}

void MainWindow::loadFile(QString path){
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly))
        QMessageBox::information(0, "info", file.errorString());

    QTextStream in(&file);

    ui->textEdit->setText(in.readAll());
}

void MainWindow::saveFile(QString path){
    QString output = ui->textEdit_2->toPlainText();
    if (path.isEmpty())
        return;
    else {
        QFile file(path);
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::information(this, tr("Unable to open file"),
                                     file.errorString());
            return;
        }
        QTextStream out(&file);
        out << output;
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty())
        loadFile(fileName);
}


void MainWindow::on_pushButton_3_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this);
    if(!fileName.isEmpty())
        saveFile(fileName);
}

