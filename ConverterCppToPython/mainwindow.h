#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "converterctop.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void loadFile(QString path);
    void saveFile(QString path);

    void on_openFileButton_clicked();

    void on_saveFileButton_clicked();

    void on_translateButton_clicked();

private:
    Ui::MainWindow *ui;

};
#endif // MAINWINDOW_H
