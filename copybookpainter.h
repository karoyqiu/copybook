#pragma once

class QPainter;
class QPrinter;

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
    void setChars(const QString &value) { chars_ = value; }
    void setMode(CopybookMode value) { mode_ = value; }

    virtual void paint();

private:
    void paintOneLineMode(QPainter &p) const;
    void paintOnePageMode(QPainter &p) const;
    void paintStroke(QPainter &p) const;
    void drawGrid(QPainter &p) const;
    
    static void mapSourceToTarget(QPainter &p, const QRectF &source, const QRectF &target);

private:
    QPrinter *printer_;
    CopybookMode mode_;
    int rows_;
    int columns_;
    qreal borderWidth_;
    qreal totalWidth_;
    qreal cellSize_;
    qreal rowHeight_;
    QFont font_;
    QString chars_;
    QPen border_;
    QPen cross_;
};
