#include "basicsemaphore.h"

BasicSemaphore::BasicSemaphore(size_t count)
    : m_mutex(), m_cond(), m_count(count)
{
    //c++11: lambda expression(&: 引用捕获 调用时即时访问该变量)
    m_waitCond = [&]{ return m_count > 0; };
}

BasicSemaphore::~BasicSemaphore()
{
    //no operation
}

void BasicSemaphore::notify()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    ++m_count;
    m_cond.notify_one();
}

void BasicSemaphore::wait()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_cond.wait(lock, m_waitCond);
    --m_count;
}

bool BasicSemaphore::try_wait()
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if(m_count > 0){
        --m_count;
        return true;
    }

    return false;
}

bool BasicSemaphore::wait_for(const double &wait_time_seconds)
{
    //构造以秒为单位的等待时间
    //目前程序以秒计时即可(double型的秒数); 若需要更换时间单位 请参考chrono库修改时间单位的定义
    const one_second_type waitTime(wait_time_seconds);//wait_time_seconds代表多少个1s的时间

    std::unique_lock<std::mutex> lock(m_mutex);
    bool finished = m_cond.wait_for(lock, waitTime, m_waitCond);

    if(finished){
        --m_count;
    }

    return finished;
}

bool BasicSemaphore::wait_until(const double &second, const int &min, const int &hour, const int &day, const int &month, const int &year)
{
    //构造指定的时间点(double型的秒数)
    const abs_time_point time_point = MakeAbsTimePoint(second, min, hour, day, month, year);

    std::unique_lock<std::mutex> lock(m_mutex);
    bool finished = m_cond.wait_until(lock, time_point, m_waitCond);

    if(finished){
        --m_count;
    }

    return finished;
}

BasicSemaphore::abs_time_point BasicSemaphore::MakeAbsTimePoint(const double &second, const int &min, const int &hour, const int &day, const int &month, const int &year)
{
    //构造指定的绝对时间点(秒的部分由于tm_sec为int, 损失小数部分)
    //用户确保时间的正确性
    std::tm tmTimePoint;
    tmTimePoint.tm_sec = second;//int型tm_sec
    tmTimePoint.tm_min = min;
    tmTimePoint.tm_hour = hour;
    tmTimePoint.tm_mday = day;
    tmTimePoint.tm_mon = month - 1;//tm_mon=0 1月
    tmTimePoint.tm_year = year - 1900;//tm_year=0 1900年
    std::time_t timeTimePoint = std::mktime(&tmTimePoint);
    abs_time_point time_point = std::chrono::system_clock::from_time_t(timeTimePoint);

    //补回损失的小数部分
    const one_second_type leftSeocnd(second - tmTimePoint.tm_sec);
    time_point += leftSeocnd;

    return time_point;
}
