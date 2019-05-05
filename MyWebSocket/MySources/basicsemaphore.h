#ifndef BASICSEMAPHORE_H
#define BASICSEMAPHORE_H

/* 为确保wait阻塞时的退出, 请在用户结束时及时调用notify()退出wait()
 * 并且, 在用户端的wait()之后判断一个条件变量, 若是退出标志则直接退出
 * eg.
 * Foo::~Foo(){ run=false; sem.notify(); }
 * Foo::Run() { sem.wait(); if(!run) return; //The operations... } */

/* 信号量的C++封装 用于消息发送的同步
 * 参考生产者-消费者模型
 *  生产者 生产消息, 消息压入发送队列 nofity()
 *  消费者 消费消息, 发送线程执行实际的发送 wait() */

#include <mutex>
#include <chrono>
#include <functional>
#include <condition_variable>

class BasicSemaphore
{
public:
    explicit BasicSemaphore(size_t count=0);
    ~BasicSemaphore();

    //通知等待线程 资源增加
    void notify();

    //阻塞等待可用资源
    void wait();

    //非阻塞等待可用资源(有可用资源=取资源并立即返回true 无可用资源=立即返回false)
    bool try_wait();

    //阻塞等待可用资源一段时间(wait_time_seconds 可带小数的秒计算) 超时后直接返回false
    bool wait_for(const double& wait_time_seconds);

    //阻塞等待可用资源直到某一时间点 超时后直接返回false
    //时间点由 秒(可带小数)-分-时-天-月-年 表示
    bool wait_until(const double& second, const int& min, const int& hour, const int& day, const int& month, const int& year);

private:
    //单位秒的定义 可带小数部分的秒(wait_for wait_until使用)
    typedef std::chrono::duration<double, std::ratio<1> > one_second_type;
    //绝对时间点的定义 可带小数部分的秒(wait_until使用)
    typedef std::chrono::time_point<std::chrono::system_clock, one_second_type> abs_time_point;

    //构造绝对时间点(wait_until使用)
    abs_time_point MakeAbsTimePoint(const double& second, const int& min, const int& hour, const int& day, const int& month, const int& year);

private:
    //disbale these functions
    BasicSemaphore(const BasicSemaphore&) = delete;
    BasicSemaphore(BasicSemaphore&&) = delete;
    BasicSemaphore& operator=(const BasicSemaphore&) = delete;
    BasicSemaphore& operator=(BasicSemaphore&&) = delete;

private:
    std::mutex m_mutex;//互斥量
    std::condition_variable m_cond;//条件变量
    size_t m_count;//资源计数
    std::function<bool()> m_waitCond;//等待条件
};

#endif // BASICSEMAPHORE_H
