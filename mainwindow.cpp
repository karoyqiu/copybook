#include "mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    loadSettings();
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    saveSettings();
    QMainWindow::closeEvent(event);
}


void MainWindow::loadSettings()
{
    QSettings settings;
    restoreGeometry(settings.value(QS("geo")).toByteArray());
    restoreState(settings.value(QS("state")).toByteArray());
    ui->comboPaperSize->setCurrentIndex(settings.value(QS("paperSize")).toInt());
    ui->comboUnit->setCurrentIndex(settings.value(QS("unit")).toInt());
    ui->comboMode->setCurrentIndex(settings.value(QS("mode")).toInt());
    ui->comboFont->setCurrentFont(settings.value(QS("font")).toString());
    ui->editChars->setText(settings.value(QS("chars")).toString());
    ui->spinMarginLeft->setValue(settings.value(QS("ml")).toDouble());
    ui->spinMarginTop->setValue(settings.value(QS("mt")).toDouble());
    ui->spinMarginRight->setValue(settings.value(QS("mr")).toDouble());
    ui->spinMarginBotom->setValue(settings.value(QS("mb")).toDouble());
    ui->spinColumns->setValue(settings.value(QS("col")).toInt());
    ui->spinRows->setValue(settings.value(QS("row")).toInt());
}


void MainWindow::saveSettings() const
{
    QSettings settings;
    settings.setValue(QS("geo"), saveGeometry());
    settings.setValue(QS("state"), saveState());
    settings.setValue(QS("paperSize"), ui->comboPaperSize->currentIndex());
    settings.setValue(QS("unit"), ui->comboUnit->currentIndex());
    settings.setValue(QS("mode"), ui->comboMode->currentIndex());
    settings.setValue(QS("font"), ui->comboFont->currentFont().family());
    settings.setValue(QS("chars"), ui->editChars->text());
    settings.setValue(QS("ml"), ui->spinMarginLeft->value());
    settings.setValue(QS("mt"), ui->spinMarginTop->value());
    settings.setValue(QS("mr"), ui->spinMarginRight->value());
    settings.setValue(QS("mb"), ui->spinMarginBotom->value());
    settings.setValue(QS("col"), ui->spinColumns->value());
    settings.setValue(QS("row"), ui->spinRows->value());
}
