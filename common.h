#pragma once
#include <QString>

#define QS(x)   QStringLiteral(x)


inline QLatin1String QL(const char *s)
{
    return QLatin1String(s);
}

inline QLatin1String QL(const QByteArray &s)
{
    return QLatin1String(s);
}

inline QLatin1String QL(const char *f, const char *l)
{
    return QLatin1String(f, l);
}

inline QLatin1String QL(const char *s, int n)
{
    return QLatin1String(s, n);
}

inline QLatin1Char QL(char ch)
{
    return QLatin1Char(ch);
}
