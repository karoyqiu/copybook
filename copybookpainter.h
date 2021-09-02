#pragma once

class QPainter;
class QPrinter;


class CopybookPainter
{
public:
    explicit CopybookPainter(QPrinter *printer);
    virtual ~CopybookPainter();

    void setDimension(int rows, int columns) { rows_ = rows; columns_ = columns; }
    void setFont(const QFont &value) { font_ = value; }
    void setChars(const QString &value) { chars_ = value; }

    virtual void paint();

private:
    QPrinter *printer_;
    QPainter *p_;
    int rows_;
    int columns_;
    QFont font_;
    QString chars_;
};

