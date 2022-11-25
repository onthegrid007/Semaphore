#ifndef SEMAPHORE_H_
#define SEMAPHORE_H_

#include "vendor/Singleton/noncopyable.h"
#include "vendor/Singleton/nonmoveable.h"
#include "vendor/Singleton/inline_abi_macros.h"
#include <condition_variable>
#include <functional>

class Semaphore : public NonCopyable, public NonMovable {
    public:
    typedef std::function<bool(const int64_t cVal, const int64_t CInitVal)> WaitFunc;
    typedef std::unique_lock<std::mutex> CVLock;
    typedef std::lock_guard<std::mutex> TLock;
    Semaphore(int64_t i = 0);
    ~Semaphore();
    Semaphore& inc(int64_t i = 0);
    Semaphore& dec(int64_t i = 0);
    void notify();
    void notify_single();
    void waitFor(WaitFunc&& retBool);
    void waitForI(int64_t i);
    void wait();
    void reset();
    Semaphore& set(int64_t i);
    operator int64_t();
    int64_t operator+(const Semaphore& other);
    int64_t operator-(const Semaphore& other);
    Semaphore& operator+=(const int64_t i);
    Semaphore& operator-=(const int64_t i);
    Semaphore& operator++();
    Semaphore& operator--();

    private:
    std::mutex m_M;
    std::condition_variable m_CV;
    int64_t m_CInit;
    int64_t m_C;
};
#endif