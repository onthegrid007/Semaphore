/*
*   BSD 3-Clause License, see file labled 'LICENSE' for the full License.
*   Copyright (c) 2023, Peter Ferranti
*   All rights reserved.
*/

#ifndef SEMAPHORE_HPP_
#define SEMAPHORE_HPP_

#include "vendor/Singleton/noncopyable.h"
#include <condition_variable>
#include <functional>

class Semaphore : public NonCopyable {
    public:
    typedef std::uint64_t CType;
    typedef std::function<bool(const CType cVal, const CType CInitVal)> WaitFunc;
    typedef std::unique_lock<std::mutex> CVLock;
    typedef std::lock_guard<std::mutex> VarLock;
    
    private:
    std::mutex m_mtx;
    std::condition_variable m_cv;
    CType m_cInit;
    CType m_c;
    
    public:
    Semaphore(const CType c) :
        m_cInit(c),
        m_c(c) {}
    
    ~Semaphore() {}
    
    void waitFor(WaitFunc spin) {
        CVLock lock(m_mtx);
        m_cv.wait(lock, [&](){ return spin(m_c, m_cInit); });
    }
    
    void waitForC(const CType c) {
        waitFor([&](const CType cVal, const CType cInitVal){ return cVal == c; });
    }
    
    void waitForInit() {
        waitFor([&](const CType cVal, const CType cInitVal){ return cVal == cInitVal; });
    }
    
    const bool waitFor(WaitFunc spin, const CType timeoutInMSec, CType timeoutLimit) {
        CVLock lock(m_mtx);
        auto until{std::chrono::steady_clock::now() + std::chrono::milliseconds(timeoutInMSec)};
        while(!spin(m_c, m_cInit)) {
            if(m_cv.wait_until(lock, until) == std::cv_status::timeout) {
                if(!(timeoutLimit -= 1)) {
                    return true;
                }
            }
            until = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeoutInMSec);
        }
        return false;
    }
    
    void spinAll() {
        m_cv.notify_all();
    }
    
    void spinOne() {
        m_cv.notify_one();
    }
    
    void inc() {
        VarLock lock(m_mtx);
        m_c += 1;
        spinAll();
    }
    
    void dec() {
        VarLock lock(m_mtx);
        m_c -= 1;
        spinAll();
    }
    
    template<bool ignoreState = true>
    void reset() {
        if constexpr(!ignoreState) {
            waitForInit();
        }
        else {
            m_c = m_cInit;
        }

    }
    
    template<bool ignoreState = true>
    void reset(const CType c) {
        if constexpr(!ignoreState) {
            waitForInit();
        }
        else {
            m_c = (m_cInit = c);
        }
    }
};
#endif