#include "strokegraphics.h"


void StrokeGraphics::loadFromFile(const QString &filename)
{
    QFile file(filename);

    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        qCritical() << "Failed to open file" << filename << file.errorString();
        return;
    }

    strokes_.clear();

    while (!file.atEnd())
    {
        auto line = file.readLine();
        auto json = QJsonDocument::fromJson(line);
        auto ch = json[QL("character")].toString();
        Q_ASSERT(ch.length() == 1);

        QStringList strokes;

        for (const auto &s : json[QL("strokes")].toArray())
        {
            strokes.append(s.toString());
        }

        strokes_.insert(ch.at(0), strokes);
    }
}


QVector<QPainterPath> StrokeGraphics::strokesFor(const QChar &ch) const
{
    const auto strokes = strokes_.value(ch);
    QVector<QPainterPath> list;

    if (Q_UNLIKELY(strokes.isEmpty()))
    {
        qWarning() << "No stroke for" << ch;
    }
    else
    {
        for (const auto &str : strokes)
        {
            list << strokeToPath(str);
        }
    }

    return list;
}


StrokeGraphics *StrokeGraphics::global()
{
    static StrokeGraphics instance;
    return &instance;
}


QPainterPath StrokeGraphics::strokeToPath(const QString &stroke)
{
    auto cmds = stroke.split(QL(' '));
    QPainterPath path;

    for (int i = 0; i < cmds.size(); i++)
    {
        const auto &cmd = cmds.at(i);
        Q_ASSERT(cmd.length() == 1);
        auto ch = cmd.at(0).toLatin1();

        switch (ch)
        {
        case 'M':
            {
                auto pt = readPoint(cmds, i);
                path.moveTo(pt);
            }
            break;

        case 'L':
            {
                auto pt = readPoint(cmds, i);
                path.lineTo(pt);
            }
            break;

        case 'Q':
            {
                auto c = readPoint(cmds, i);
                auto end = readPoint(cmds, i);
                path.quadTo(c, end);
            }
            break;

        case 'C':
            {
                auto c1 = readPoint(cmds, i);
                auto c2 = readPoint(cmds, i);
                auto end = readPoint(cmds, i);
                path.cubicTo(c1, c2, end);
            }
            break;

        case 'Z':
        case 'z':
            path.closeSubpath();
            break;

        default:
            qCritical() << "Unknown command:" << cmd;
            Q_UNREACHABLE();
            break;
        }
    }

    return path;
}


QPointF StrokeGraphics::readPoint(const QStringList &cmds, int &from)
{
    auto x = cmds.at(from + 1).toDouble();
    auto y = cmds.at(from + 2).toDouble();
    from += 2;
    return { x, y };
}
