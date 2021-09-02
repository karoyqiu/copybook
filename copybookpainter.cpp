#include "copybookpainter.h"


CopybookPainter::CopybookPainter(QPrinter *printer)
    : printer_(printer)
    , p_(nullptr)
    , rows_(0)
    , columns_(0)
{
    Q_ASSERT(printer != nullptr);
}


CopybookPainter::~CopybookPainter()
{
    delete p_;
}


void CopybookPainter::paint()
{
    Q_ASSERT(p_ == nullptr && rows_ > 0 && columns_ > 0 && !chars_.isEmpty());

    // 统一单位为点，计算每格大小
    auto layout = printer_->pageLayout();
    auto paintRect = layout.paintRectPoints();
    QRectF boundry(paintRect);
    auto width = boundry.width() / columns_;
    auto height = boundry.height() / rows_;

    // 单元格大小
    auto size = qMin(width, height);
    auto half = size / 2;
    // 行间距
    auto spacing = (height - size) * rows_ / (rows_ - 1);
    // 实际行高
    height = size + spacing;
    width = boundry.width();

    QPen border(Qt::SolidPattern, 2);
    QPen cross(Qt::SolidPattern, 1, Qt::DashLine);
    QPen shadow(Qt::Dense5Pattern, 1);

    p_ = new QPainter(printer_);
    paintRect.moveTo(0, 0);
    p_->setWindow(paintRect);

    for (int row = 0; row < rows_; row++)
    {
        p_->save();

        // 画虚线
        auto y = height * row;
        p_->setPen(cross);
        p_->drawLine(0, y + half, width, y + half);

        // 竖线
        for (int col = 0; col < columns_; col++)
        {
            auto x = size * col + half;
            p_->drawLine(x, y, x, y + size);
        }

        // 画边框
        p_->setPen(border);

        // 上下横线
        p_->drawLine(0, y, width, y);
        p_->drawLine(0, y + size, width, y + size);

        // 左右竖线
        for (int col = 0; col <= columns_; col++)
        {
            auto x = size * col;
            p_->drawLine(x, y, x, y + size);
        }

        // 画字
        font_.setPixelSize(qRound(size * 0.9));
        p_->setFont(font_);
        p_->setPen(Qt::SolidLine);

        auto ch = chars_.mid(row % chars_.length(), 1);

        for (int col = 0; col < columns_; col++)
        {
            auto x = size * col;
            QRectF rect(x, y, size, size);
            p_->setPen(col == 0 ? border : shadow);
            p_->drawText(rect, Qt::AlignCenter, ch);
        }

        p_->restore();
    }
}
