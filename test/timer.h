// Copyright (c) 2019 Greg Griffith
// Copyright (c) 2019 The Bitcoin Unlimited developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <inttypes.h>
#include <stdint.h>
#include <string>

std::chrono::steady_clock::time_point GetTimeNow();

int64_t GetDuration(std::chrono::steady_clock::time_point start, std::chrono::steady_clock::time_point end);


void MilliSleep(int64_t n);

#endif // TIMER_H
