#include <QApplication>
#include <QLocale>
#include <QTranslator>

#include "mainwindow.h"


int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_DisableWindowContextHelpButton);

    QApplication a(argc, argv);
    QApplication::setApplicationName(QS("copybook"));
    QApplication::setOrganizationName(QS("ubesthelp"));
    QApplication::setOrganizationDomain(QS("ubesthelp.com"));

    QTranslator translator;
    QLocale locale;

    if (translator.load(locale, QS("copybook"), QS("_"), QS(":/i18n")))
    {
        a.installTranslator(&translator);
    }

    QApplication::setApplicationDisplayName(QApplication::translate("main", "Copybook"));

    MainWindow w;
    w.show();

    return a.exec();
}
