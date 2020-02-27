// Copyright (c) 2019 Greg Griffith
// Copyright (c) 2019 The Bitcoin Unlimited developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "timer.h"

#include <atomic>
#include <ratio>

#include <thread>

std::chrono::steady_clock::time_point GetTimeNow()
{
    std::chrono::steady_clock::now();
}

int64_t GetDuration(std::chrono::steady_clock::time_point start, std::chrono::steady_clock::time_point end)
{
    std::chrono::duration<int64_t, std::nano> time_span = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    return time_span.count();
}

void MilliSleep(int64_t n)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(n));
}
