#ifndef HIGHPRECISIONTIMER_H
#define HIGHPRECISIONTIMER_H

#include <QTimer>
#include <QObject>

class HighPrecisionTimer : public QObject
{
    Q_OBJECT
public:
    explicit HighPrecisionTimer(QObject *parent = 0);
    virtual ~HighPrecisionTimer();

    //高精度定时器 定时周期=intervalMs*multiplier
    //Qt提供20ms以内的高精度定时器 intervalMs<20
    void StartTimer(int intervalMs, int intervalMultiplier);
    void StopTimer();

signals:
    //intervalMs超时
    void intervalTimeout(int currentTimeoutCount, int intervalMultiplier);

    //intervalMs*multiplier超时
    void timeout();

private slots:
    void OnTimerOut();

private:
    int m_intervalMultiplier;
    int m_timeoutCount;

    QTimer m_timer;
};

#endif // HIGHPRECISIONTIMER_H
