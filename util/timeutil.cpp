#include "timeutil.h"

#include <QDateTime>

qint64 TimeUtil::CurrentTimestampMs()
{
    return QDateTime::currentMSecsSinceEpoch();
}
