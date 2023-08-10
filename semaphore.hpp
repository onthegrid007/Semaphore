#ifndef SEMAPHORE_HPP_
#define SEMAPHORE_HPP_

#include "vendor/Singleton/noncopyable.h"
#include "vendor/Singleton/inline_abi_macros.h"
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
    
    template<typename TP>
    const std::cv_status waitFor(WaitFunc spin, const TP& until) {
        CVLock lock(m_mtx);
        return m_cv.wait_until(lock, until, [&](){ return spin(m_c, m_cInit); });
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