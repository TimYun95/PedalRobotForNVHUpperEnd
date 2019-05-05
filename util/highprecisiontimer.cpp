#include "highprecisiontimer.h"

#include "common/printf.h"

HighPrecisionTimer::HighPrecisionTimer(QObject *parent)
    : QObject(parent)
{
    m_intervalMultiplier = 1;
    m_timeoutCount = 0;

    connect(&m_timer, SIGNAL(timeout()), this, SLOT(OnTimerOut()));
}

HighPrecisionTimer::~HighPrecisionTimer()
{
    StopTimer();
}

void HighPrecisionTimer::StartTimer(int intervalMs, int intervalMultiplier)
{
    if(intervalMs >= 20){
        PRINTF(LOG_INFO, "%s: the Qt framework provides accurate timer with interval < 20ms!\n", __func__);
        PRINTF(LOG_INFO, "%s: [%d]ms timer may be inaccurate!\n", __func__, intervalMs);
    }

    m_intervalMultiplier = intervalMultiplier;
    m_timeoutCount = 0;

    m_timer.start(intervalMs);
}

void HighPrecisionTimer::StopTimer()
{
    m_timer.stop();
}

void HighPrecisionTimer::OnTimerOut()
{
    int currentTimeoutCount = ++m_timeoutCount;
    emit intervalTimeout(currentTimeoutCount, m_intervalMultiplier);

    if(currentTimeoutCount != m_intervalMultiplier){
        return;
    }

    m_timeoutCount = 0;
    emit timeout();
}
