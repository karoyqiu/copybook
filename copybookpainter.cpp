#include "copybookpainter.h"


struct QPainterSaver
{
    explicit QPainterSaver(QPainter &p) : p_(p) { p_.save(); }
    ~QPainterSaver() { p_.restore(); }

    QPainter &p_;
};


CopybookPainter::CopybookPainter(QPrinter *printer)
    : printer_(printer)
    , mode_(CopybookMode::OneLinePerCharacter)
    , rows_(0)
    , columns_(0)
    , borderWidth_(1.5)
    , totalWidth_(0)
    , cellSize_(0)
    , rowHeight_(0)
    , border_(Qt::SolidPattern, borderWidth_)
    , cross_(Qt::SolidPattern, borderWidth_ / 2, Qt::CustomDashLine, Qt::FlatCap)
{
    Q_ASSERT(printer != nullptr);
}


CopybookPainter::~CopybookPainter()
{
}


void CopybookPainter::paint()
{
    Q_ASSERT(rows_ > 0 && columns_ > 0 && !chars_.isEmpty());

    // 统一单位为点，计算每格大小
    auto layout = printer_->pageLayout();
    auto paintRect = layout.paintRectPoints();
    QRectF boundry(paintRect);
    auto width = boundry.width() / columns_;
    auto height = boundry.height() / rows_;
    paintRect.moveTo(0, 0);

    // 单元格大小
    cellSize_ = qMin(width, height);
    totalWidth_ = cellSize_ * columns_;
    font_.setPixelSize(qRound(cellSize_ * 0.9));

    auto dash = cellSize_ / 13;
    cross_.setDashPattern({ dash, dash });

    // 行间距
    auto spacing = (height - cellSize_) * rows_ / (rows_ - 1);
    // 实际行高
    rowHeight_ = cellSize_ + spacing;

    QPainter p(printer_);
    p.setFont(font_);
    p.setWindow(paintRect);

    switch (mode_)
    {
    case CopybookMode::OneLinePerCharacter:
        paintOneLineMode(p);
        break;
    case CopybookMode::OnePagePerCharacter:
        paintOnePageMode(p);
        break;
    }
}


void CopybookPainter::paintOneLineMode(QPainter &p) const
{
    drawGrid(p);

    for (int row = 0; row < rows_; row++)
    {
        QPainterSaver ps(p);

        // 画字
        auto y = rowHeight_ * row;
        auto ch = chars_.mid(row % chars_.length(), 1);

        for (int col = 0; col < columns_; col++)
        {
            auto x = cellSize_ * col;
            QRectF rect(x, y, cellSize_, cellSize_);
            p.setPen(col == 0 ? Qt::black : Qt::lightGray);
            p.drawText(rect, Qt::AlignCenter, ch);
        }
    }
}


void CopybookPainter::paintOnePageMode(QPainter &p) const
{
    for (int i = 0; i < chars_.length(); i++)
    {
        if (i > 0)
        {
            printer_->newPage();
        }

        drawGrid(p);

        // 画字
        auto ch = chars_.mid(i, 1);

        for (int row = 0; row < rows_; row++)
        {
            QPainterSaver ps(p);

            auto y = rowHeight_ * row;

            for (int col = 0; col < columns_; col++)
            {
                auto x = cellSize_ * col;
                QRectF rect(x, y, cellSize_, cellSize_);
                p.setPen(col == 0 ? Qt::black : Qt::lightGray);
                p.drawText(rect, Qt::AlignCenter, ch);
            }
        }
    }
}


void CopybookPainter::drawGrid(QPainter &p) const
{
    auto half = cellSize_ / 2;

    for (int row = 0; row < rows_; row++)
    {
        QPainterSaver ps(p);

        // 画虚线
        p.setPen(cross_);
        auto y = rowHeight_ * row;

        for (int col = 0; col < columns_; col++)
        {
            auto x = cellSize_ * col;
            p.drawLine(x, y + half, x + cellSize_, y + half);
            p.drawLine(x + half, y, x + half, y + cellSize_);
        }

        // 画边框
        p.setPen(border_);

        // 上下横线
        p.drawLine(0, y, totalWidth_, y);
        p.drawLine(0, y + cellSize_, totalWidth_, y + cellSize_);

        // 竖线
        for (int col = 0; col <= columns_; col++)
        {
            auto x = cellSize_ * col;
            p.drawLine(x, y, x, y + cellSize_);
        }
    }
}
