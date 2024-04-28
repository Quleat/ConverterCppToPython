#include "mainwindow.h"
#include "./ui_mainwindow.h"
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

void MainWindow::loadFile(QString path){
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly))
        QMessageBox::information(0, "info", file.errorString());

    QTextStream in(&file);

    ui->inputBox->setText(in.readAll());
}

void MainWindow::saveFile(QString path){
    QString output = ui->outputBox->toPlainText();
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

void MainWindow::on_openFileButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty())
        loadFile(fileName);
}


void MainWindow::on_saveFileButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this);
    if(!fileName.isEmpty())
        saveFile(fileName);
}


void MainWindow::on_translateButton_clicked()
{
    QString text = ui->inputBox->toPlainText();
    if(text.isEmpty()){
        QMessageBox::information(0, "info", "The field is empty\nThere's nothing to translate.");
        return;
    }

    string str = text.toStdString();

    Converter conv;
    try{
        if(str.size() > 5000)
            throw exceedingSymbolsException();
        stringstream stream = conv.TranslateOuter(str);
        QString output(stream.str().c_str());
        ui->outputBox->setText(output);
    }catch(exceedingSymbolsException e){
        auto str = e.what();
        string message = str;
        QString qStr = QString::fromStdString(message);
        QMessageBox::information(0, "info", "You have used too many symbols, friend! " + qStr);
    }
    catch (exception &e){
        auto str = e.what();
        string errorMessage = str;
        QString qStr = QString::fromStdString(errorMessage);
        QMessageBox::critical(0, "error", "The error has occured while translating the code:\n" + qStr);
    }
}

