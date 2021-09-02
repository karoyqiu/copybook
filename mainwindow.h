#pragma once
#include <QMainWindow>

class QPrinter;
class QPrintPreviewWidget;

namespace Ui { class MainWindow; }


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow();

protected:
    virtual void closeEvent(QCloseEvent *event) override;

private:
    void loadSettings();
    void saveSettings() const;

    QPrinter *createPrinter() const;
    void setPrinterParams(QPrinter *printer) const;

    void fillPageSizes();
    void adjustPrinterParams();
    void updatePreview();
    void createPreviewWidget();

    void draw(QPrinter *printer);
    void print();

private:
    Ui::MainWindow *ui;
    QPrinter *printer_;
    QPrintPreviewWidget *previewer_;
};
