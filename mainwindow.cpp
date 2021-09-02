#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "copybookpainter.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , printer_(nullptr)
    , previewer_(nullptr)
{
    ui->setupUi(this);

    // 单独加载打印机选项
    QSettings settings;
    ui->comboPrinter->addItems(QPrinterInfo::availablePrinterNames());
    ui->comboPrinter->setCurrentText(settings.value(QS("printer")).toString());

    // 填充纸张大小
    fillPageSizes();

    // 加载其它选项
    loadSettings();

    // 构建打印机和预览
    createPreviewWidget();

    // 连接信号
    connect(ui->comboPrinter, &QComboBox::currentTextChanged, this, &MainWindow::fillPageSizes);
    connect(ui->comboPrinter, &QComboBox::currentTextChanged, this, &MainWindow::createPreviewWidget);
    connect(ui->comboPageSize, &QComboBox::currentTextChanged, this, &MainWindow::adjustPrinterParams);
    connect(ui->comboUnit, &QComboBox::currentTextChanged, this, &MainWindow::adjustPrinterParams);
    connect(ui->spinMarginLeft, &QDoubleSpinBox::editingFinished, this, &MainWindow::adjustPrinterParams);
    connect(ui->spinMarginTop, &QDoubleSpinBox::editingFinished, this, &MainWindow::adjustPrinterParams);
    connect(ui->spinMarginRight, &QDoubleSpinBox::editingFinished, this, &MainWindow::adjustPrinterParams);
    connect(ui->spinMarginBottom, &QDoubleSpinBox::editingFinished, this, &MainWindow::adjustPrinterParams);
    connect(ui->spinColumns, &QDoubleSpinBox::editingFinished, this, &MainWindow::updatePreview);
    connect(ui->spinRows, &QDoubleSpinBox::editingFinished, this, &MainWindow::updatePreview);
    connect(ui->comboFont, &QComboBox::currentTextChanged, this, &MainWindow::updatePreview);
    connect(ui->editChars, &QLineEdit::editingFinished, this, &MainWindow::updatePreview);
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
    ui->comboPageSize->setCurrentIndex(settings.value(QS("paperSize")).toInt());
    ui->comboUnit->setCurrentIndex(settings.value(QS("unit")).toInt());
    ui->comboMode->setCurrentIndex(settings.value(QS("mode")).toInt());
    ui->comboFont->setCurrentFont(settings.value(QS("font")).toString());
    ui->editChars->setText(settings.value(QS("chars")).toString());
    ui->spinMarginLeft->setValue(settings.value(QS("ml")).toDouble());
    ui->spinMarginTop->setValue(settings.value(QS("mt")).toDouble());
    ui->spinMarginRight->setValue(settings.value(QS("mr")).toDouble());
    ui->spinMarginBottom->setValue(settings.value(QS("mb")).toDouble());
    ui->spinColumns->setValue(settings.value(QS("col")).toInt());
    ui->spinRows->setValue(settings.value(QS("row")).toInt());
}


void MainWindow::saveSettings() const
{
    QSettings settings;
    settings.setValue(QS("geo"), saveGeometry());
    settings.setValue(QS("state"), saveState());
    settings.setValue(QS("printer"), ui->comboPrinter->currentText());
    settings.setValue(QS("paperSize"), ui->comboPageSize->currentIndex());
    settings.setValue(QS("unit"), ui->comboUnit->currentIndex());
    settings.setValue(QS("mode"), ui->comboMode->currentIndex());
    settings.setValue(QS("font"), ui->comboFont->currentFont().family());
    settings.setValue(QS("chars"), ui->editChars->text());
    settings.setValue(QS("ml"), ui->spinMarginLeft->value());
    settings.setValue(QS("mt"), ui->spinMarginTop->value());
    settings.setValue(QS("mr"), ui->spinMarginRight->value());
    settings.setValue(QS("mb"), ui->spinMarginBottom->value());
    settings.setValue(QS("col"), ui->spinColumns->value());
    settings.setValue(QS("row"), ui->spinRows->value());
}


void MainWindow::fillPageSizes()
{
    ui->comboPageSize->clear();
    auto info = QPrinterInfo::printerInfo(ui->comboPrinter->currentText());

    for (const auto &ps : info.supportedPageSizes())
    {
        ui->comboPageSize->addItem(ps.name(), ps.id());
    }
}


void MainWindow::adjustPrinterParams()
{
    Q_ASSERT(printer_ != nullptr);

    QPageLayout layout;
    layout.setOrientation(QPageLayout::Portrait);

    auto psId = static_cast<QPageSize::PageSizeId>(ui->comboPageSize->currentData().toInt());
    layout.setPageSize(QPageSize(psId));

    auto unit = static_cast<QPageLayout::Unit>(ui->comboUnit->currentIndex());
    layout.setUnits(unit);
    layout.setMargins({ ui->spinMarginLeft->value(), ui->spinMarginTop->value(),
                        ui->spinMarginRight->value(), ui->spinMarginBottom->value() });

    printer_->setPageLayout(layout);

    updatePreview();
}


void MainWindow::updatePreview()
{
    if (previewer_ != nullptr)
    {
        previewer_->updatePreview();
    }
}


void MainWindow::createPreviewWidget()
{
    // 删除原来的组件
    if (previewer_ != nullptr)
    {
        Q_ASSERT(printer_ != nullptr);
        ui->centralLayout->removeWidget(previewer_);
        previewer_->disconnect(this);
        previewer_->deleteLater();
        delete printer_;
        printer_ = nullptr;
        previewer_ = nullptr;
    }

    // 构建新打印机组件
    auto info = QPrinterInfo::printerInfo(ui->comboPrinter->currentText());
    qInfo() << "Current printer:" << info;

    printer_ = new QPrinter(info, QPrinter::PrinterResolution);
    adjustPrinterParams();

    // 构建预览组件
    previewer_ = new QPrintPreviewWidget(printer_, this);
    previewer_->setAllPagesViewMode();
    previewer_->setZoomMode(QPrintPreviewWidget::FitInView);
    connect(previewer_, &QPrintPreviewWidget::paintRequested, this, &MainWindow::draw);
    ui->centralLayout->addWidget(previewer_, 1);
}


void MainWindow::draw(QPrinter *printer)
{
    CopybookPainter cp(printer);
    cp.setDimension(ui->spinRows->value(), ui->spinColumns->value());
    cp.setFont(ui->comboFont->currentFont());
    cp.setChars(ui->editChars->text());
    cp.paint();
}
