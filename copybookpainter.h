#pragma once

class QPainter;
class QPrinter;

/// 格
enum class GridType
{
    /// 田字格
    Tian,
    /// 四线三格
    FourLines,
};

/// 字帖模式
enum class CopybookMode
{
    /// 每字一行
    OneLinePerCharacter,
    /// 每字一页
    OnePagePerCharacter,
    /// 笔画分解，每字一页
    Stroke,
};


class CopybookPainter
{
public:
    explicit CopybookPainter(QPrinter *printer);
    virtual ~CopybookPainter();

    void setDimension(int rows, int columns) { rows_ = rows; columns_ = columns; }
    void setFont(const QFont &value) { font_ = value; }
    void setChars(const QString &value) { chars_ = splitChars(value); }
    void setMode(CopybookMode value) { mode_ = value; }
    void setGrid(GridType value) { grid_ = value; }
    void setScale(qreal value) { scale_ = value * 0.85; }       // 默认四周留空
    void setOffset(const QPointF &value) { offset_ = value; }
    void setOffset(qreal x, qreal y) { offset_.setX(x); offset_.setY(y); }

    virtual void paint();

private:
    void paintOneLineMode(QPainter &p) const;
    void paintOnePageMode(QPainter &p) const;
    void paintStroke(QPainter &p) const;
    void drawGrid(QPainter &p) const;
    void drawTianGrid(QPainter &p) const;
    void drawFourLines(QPainter &p) const;
    QRectF cellRect(int row, int col) const;
    QRectF cellRect(qreal x, qreal y) const;

    static QStringList splitChars(const QString &s);

private:
    QPrinter *printer_;
    CopybookMode mode_;
    int rows_;
    int columns_;
    GridType grid_;
    qreal borderWidth_;
    qreal totalWidth_;
    qreal cellSize_;
    qreal rowHeight_;
    qreal scale_;
    qreal margin_;
    QPointF offset_;
    QFont font_;
    QStringList chars_;
    QPen border_;
    QPen cross_;
};
