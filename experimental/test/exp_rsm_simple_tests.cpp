// Copyright (c) 2019 Greg Griffith
// Copyright (c) 2019 The Bitcoin Unlimited developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "exp_recursive_shared_mutex.h"
#include "test/test_cxx_rsm.h"
#include "test/timer.h"

#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(exp_rsm_simple_tests, TestSetup)

exp_recursive_shared_mutex rsm;

void rsm_lock_unlock()
{
    // exclusive lock once
    rsm.lock();

// try to unlock_shared an exclusive lock
// we should error here because exclusive locks can
// be not be unlocked by shared_ unlock method
#ifdef RSM_DEBUG_ASSERTION
    BOOST_CHECK_THROW(rsm.unlock_shared(), std::logic_error);
#endif

    // unlock exclusive lock

    BOOST_CHECK_NO_THROW(rsm.unlock());

    // exclusive lock once
    rsm.lock();

    // try to unlock exclusive lock
    BOOST_CHECK_NO_THROW(rsm.unlock());

#ifdef RSM_DEBUG_ASSERTION
    // try to unlock exclusive lock more times than we locked
    BOOST_CHECK_THROW(rsm.unlock(), std::logic_error);
#endif

    // test complete
}

void rsm_lock_shared_unlock_shared()
{
    // lock shared
    rsm.lock_shared();

#ifdef RSM_DEBUG_ASSERTION
    // try to unlock exclusive when we only have shared
    BOOST_CHECK_THROW(rsm.unlock(), std::logic_error);
#endif

    // unlock shared
    rsm.unlock_shared();

#ifdef RSM_DEBUG_ASSERTION
    // we should error here because we are unlocking more times than we locked
    BOOST_CHECK_THROW(rsm.unlock_shared(), std::logic_error);
#endif

    // test complete
}

// basic try_lock tests
void rsm_try_lock()
{
    // try lock
    rsm.try_lock();

#ifdef RSM_DEBUG_ASSERTION
    // try to unlock_shared an exclusive lock
    // we should error here because exclusive locks can
    // be not be unlocked by shared_ unlock method
    BOOST_CHECK_THROW(rsm.unlock_shared(), std::logic_error);
#endif

    // unlock exclusive lock
    BOOST_CHECK_NO_THROW(rsm.unlock());

    // try lock
    rsm.try_lock();

    // try to unlock exclusive lock
    BOOST_CHECK_NO_THROW(rsm.unlock());

#ifdef RSM_DEBUG_ASSERTION
    // try to unlock exclusive lock more times than we locked
    BOOST_CHECK_THROW(rsm.unlock(), std::logic_error);
#endif

    // test complete
}

// basic try_lock_shared tests
 void rsm_try_lock_shared()
{
    // try lock shared
    rsm.try_lock_shared();

#ifdef RSM_DEBUG_ASSERTION
    // unlock exclusive while we have shared lock
    BOOST_CHECK_THROW(rsm.unlock(), std::logic_error);
#endif

    // unlock shared
    BOOST_CHECK_NO_THROW(rsm.unlock_shared());

#ifdef RSM_DEBUG_ASSERTION
    // we should error here because we are unlocking more times than we locked
    BOOST_CHECK_THROW(rsm.unlock_shared(), std::logic_error);
#endif

    // test complete
}

// test locking recursively 100 times for each lock type
void rsm_100_lock_test()
{
    uint8_t i = 0;
    // lock
    while (i < 100)
    {
        BOOST_CHECK_NO_THROW(rsm.lock());
        ++i;
    }

    while (i > 0)
    {
        BOOST_CHECK_NO_THROW(rsm.unlock());
        --i;
    }

    // lock_shared
    while (i < 100)
    {
        BOOST_CHECK_NO_THROW(rsm.lock_shared());
        ++i;
    }

    while (i > 0)
    {
        BOOST_CHECK_NO_THROW(rsm.unlock_shared());
        --i;
    }

    // try_lock
    while (i < 100)
    {
        BOOST_CHECK_NO_THROW(rsm.try_lock());
        ++i;
    }

    while (i > 0)
    {
        BOOST_CHECK_NO_THROW(rsm.unlock());
        --i;
    }

    // try_lock_shared
    while (i < 100)
    {
        BOOST_CHECK_NO_THROW(rsm.try_lock_shared());
        ++i;
    }

    while (i > 0)
    {
        BOOST_CHECK_NO_THROW(rsm.unlock_shared());
        --i;
    }

    // test complete
}


BOOST_AUTO_TEST_CASE(rsm_simple_tests)
{
    rsm_lock_unlock();
    rsm_lock_shared_unlock_shared();
    rsm_try_lock();
    rsm_try_lock_shared();
    rsm_100_lock_test();
}

BOOST_AUTO_TEST_CASE(rsm_simple_tests_perf)
{
    uint32_t run = 0;
    std::chrono::steady_clock::time_point startTime_1 = GetTimeNow();
    for (run = 0; run < 100000; ++run)
    {
        rsm_lock_unlock();
    }
    std::chrono::steady_clock::time_point endTime_1 = GetTimeNow();
    int64_t duration_1 = GetDuration(startTime_1, endTime_1);
    printf("EXP rsm_lock_unlock took %" PRId64 " microseconds \n", duration_1);

    std::chrono::steady_clock::time_point startTime_2 = GetTimeNow();
    for (run = 0; run < 100000; ++run)
    {
        rsm_lock_shared_unlock_shared();
    }
    std::chrono::steady_clock::time_point endTime_2 = GetTimeNow();
    int64_t duration_2 = GetDuration(startTime_2, endTime_2);
    printf("EXP rsm_lock_shared_unlock_shared took %" PRId64 " microseconds \n", duration_2);

    std::chrono::steady_clock::time_point startTime_3 = GetTimeNow();
    for (run = 0; run < 100000; ++run)
    {
        rsm_try_lock();
    }
    std::chrono::steady_clock::time_point endTime_3 = GetTimeNow();
    int64_t duration_3 = GetDuration(startTime_3, endTime_3);
    printf("EXP rsm_try_lock took %" PRId64 " microseconds \n", duration_3);

    std::chrono::steady_clock::time_point startTime_4 = GetTimeNow();
    for (run = 0; run < 100000; ++run)
    {
        rsm_try_lock_shared();
    }
    std::chrono::steady_clock::time_point endTime_4 = GetTimeNow();
    int64_t duration_4 = GetDuration(startTime_4, endTime_4);
    printf("EXP rsm_try_lock_shared took %" PRId64 " microseconds \n", duration_4);

    std::chrono::steady_clock::time_point startTime_5 = GetTimeNow();
    for (run = 0; run < 10000; ++run)
    {
        rsm_100_lock_test();
    }
    std::chrono::steady_clock::time_point endTime_5 = GetTimeNow();
    int64_t duration_5 = GetDuration(startTime_5, endTime_5);
    printf("EXP rsm_100_lock_test took %" PRId64 " microseconds \n", duration_5);
}


BOOST_AUTO_TEST_SUITE_END()
