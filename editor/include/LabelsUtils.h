//
// Created by gorev on 17.10.2025.
//

#pragma once
#include <qstring.h>

namespace editor
{
inline void ToHeader1(QString &str)
{
    str.prepend("# ");
}


inline QString ToHeader1(const QString &str)
{
    return "# " + str;
}


inline void ToHeader2(QString &str)
{
    str.prepend("## ");
}


inline QString ToHeader2(const QString &str)
{
    return "## " + str;
}


inline void ToHeader3(QString &str)
{
    str.prepend("### ");
}


inline QString ToHeader3(const QString &str)
{
    return "### " + str;
}


inline void ToHeader4(QString &str)
{
    str.prepend("#### ");
}


inline QString ToHeader4(const QString &str)
{
    return "#### " + str;
}

} // namespace editor