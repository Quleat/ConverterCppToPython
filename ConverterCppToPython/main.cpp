#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <regex>
#include <sstream>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "ConverterCppToPython_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    MainWindow w;
    w.show();
    return a.exec();
}
