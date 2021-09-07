#pragma once


class StrokeGraphics
{
public:
    void loadFromFile(const QString &filename);

    bool isEmpty() const { return strokes_.isEmpty();  }
    QVector<QPainterPath> strokesFor(const QChar &ch) const;

    static StrokeGraphics *global();

private:
    static QPainterPath strokeToPath(const QString &stroke);
    static QPointF readPoint(const QStringList &cmds, int &from);

private:
    QHash<QChar, QStringList> strokes_;
};
