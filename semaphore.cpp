/*
*   BSD 3-Clause License, see file labled 'LICENSE' for the full License.
*   Copyright (c) 2022, Peter Ferranti
*   All rights reserved.
*/

#include "semaphore.h"

Semaphore::Semaphore(int64_t i) :
	m_CInit(i),
	m_C(i) { }

Semaphore& Semaphore::inc(int64_t i) {
	return *this += i;
}

Semaphore& Semaphore::dec(int64_t i) {
	return *this -= i;
}

void Semaphore::notify() {
	m_CV.notify_all();
}

void Semaphore::notify_single() {
	m_CV.notify_one();
}

void Semaphore::waitFor(WaitFunc&& rtnBool) {
	CVLock lock(m_M);
	while(!rtnBool(m_C, m_CInit)) m_CV.wait(lock, [cVal = m_C, cInitVal = m_CInit, &rtnBool]{ return rtnBool(cVal, cInitVal); });
}

void Semaphore::waitForI(int64_t i) {
	waitFor([&, i](const int64_t cVal, const int64_t cInitVal){ return (cVal == i); });
}

void Semaphore::wait() {
	waitFor([&](const int64_t cVal, const int64_t cInitVal){ return (cVal == cInitVal); });
}

Semaphore& Semaphore::set(int64_t i) {
	wait();
	m_C = i;
	m_CInit = i;
	return *this;
}

Semaphore::operator int64_t() {
	return m_C;
}

int64_t Semaphore::operator+(const Semaphore& other) {
	return m_C + other.m_C;
}

int64_t Semaphore::operator-(const Semaphore& other) {
	return m_C - other.m_C;
}

Semaphore& Semaphore::operator+=(const int64_t i) {
	auto* lock = new TLock(m_M);
	m_C += i;
	delete lock;
	// notify();
	return *this;
}

Semaphore& Semaphore::operator-=(const int64_t i) {
	auto* lock = new TLock(m_M);
	m_C -= i;
	delete lock;
	// notify();
	return *this;
}

Semaphore& Semaphore::operator++() {
	return this->inc();
}

Semaphore& Semaphore::operator--() {
	return this->dec();
}

Semaphore::~Semaphore() {
	// wait();
}