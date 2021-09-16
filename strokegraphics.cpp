#include "strokegraphics.h"

#define DB_NAME     QS("strokedb")


StrokeGraphics::StrokeGraphics()
{
    openDatabase();
}


StrokeGraphics::~StrokeGraphics()
{
    closeDatabase();
}


void StrokeGraphics::loadFromFile(const QString &filename)
{
    QFile file(filename);

    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        qCritical() << "Failed to open file" << filename << file.errorString();
        return;
    }

    if (!QSqlDatabase::contains(DB_NAME))
    {
        auto appData = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
        QDir dir(appData);
        dir.mkpath(QS("."));

        openDatabase();
    }

    auto db = database();
    db.transaction();

    QSqlQuery query(db);
    query.prepare(QS("REPLACE INTO strokes (ch, str) VALUES (?, ?);"));

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

        query.addBindValue(ch);
        query.addBindValue(strokes.join(QL('|')));
        query.exec();
    }

    db.commit();
    db.exec(QS("VACUUM;"));
}


QVector<QPainterPath> StrokeGraphics::strokesFor(const QChar &ch) const
{
    QVector<QPainterPath> list;

    QSqlQuery query(database());
    query.setForwardOnly(true);
    query.prepare(QS("SELECT str FROM strokes WHERE ch = ?;"));
    query.addBindValue(ch);

    if (query.exec() && query.next())
    {
        const auto strokes = query.value(0).toString().splitRef(QL('|'));

        for (const auto &str : strokes)
        {
            list << strokeToPath(str);
        }
    }
    else
    {
        qWarning() << "No stroke for" << ch;
    }

    return list;
}


StrokeGraphics *StrokeGraphics::global()
{
    static StrokeGraphics instance;
    return &instance;
}


QPainterPath StrokeGraphics::strokeToPath(const QStringRef &stroke)
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


QPointF StrokeGraphics::readPoint(const QVector<QStringRef> &cmds, int &from)
{
    auto x = cmds.at(from + 1).toDouble();
    auto y = cmds.at(from + 2).toDouble();
    from += 2;
    return { x, y };
}


void StrokeGraphics::openDatabase()
{
    auto appData = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    QString file = appData % QL("/strokes.db");

    auto db = QSqlDatabase::addDatabase(QS("QSQLITE"), DB_NAME);
    db.setDatabaseName(file);

    if (db.open())
    {
        db.exec(QS("CREATE TABLE strokes (ch TEXT PRIMARY KEY, str TEXT);"));
    }
    else
    {
        qWarning() << "Failed to open stroke database:" << db.lastError();
        closeDatabase();
    }
}


void StrokeGraphics::closeDatabase()
{
    auto db = database();
    db.close();
    QSqlDatabase::removeDatabase(DB_NAME);
}


QSqlDatabase StrokeGraphics::database() const
{
    return QSqlDatabase::database(DB_NAME);
}
