#pragma once


class StrokeGraphics
{
public:
    StrokeGraphics();
    virtual ~StrokeGraphics();

    void loadFromFile(const QString &filename);

    QVector<QPainterPath> strokesFor(const QChar &ch) const;

    static StrokeGraphics *global();

private:
    static QPainterPath strokeToPath(const QStringRef &stroke);
    static QPointF readPoint(const QVector<QStringRef> &cmds, int &from);

    void openDatabase();
    void closeDatabase();
    QSqlDatabase database() const;
};
