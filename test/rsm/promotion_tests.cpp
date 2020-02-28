// Copyright (c) 2019 Greg Griffith
// Copyright (c) 2019 The Bitcoin Unlimited developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "recursive_shared_mutex.h"
#include "test/test_cxx_rsm.h"
#include "test/timer.h"

#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(rsm_promotion_tests, TestSetup)

recursive_shared_mutex rsm;
std::vector<int> rsm_guarded_vector;

void helper_fail() { BOOST_CHECK_EQUAL(rsm.try_lock(), false); }
void helper_pass()
{
    BOOST_CHECK_EQUAL(rsm.try_lock(), true);
    // unlock the try_lock
    rsm.unlock();
}
// test locking shared while holding exclusive ownership
// we should require an equal number of unlock_shared for each lock_shared
void rsm_lock_shared_while_exclusive_owner()
{
    // lock exclusive 3 times
    rsm.lock();
    rsm.lock();
    rsm.lock();

    // lock_shared twice
    rsm.lock_shared();
    rsm.lock_shared();

    // it should require 3 unlocks and 2 unlock_shareds to have another thread lock exclusive

    // dont unlock exclusive enough times
    rsm.unlock();
    rsm.unlock();
    rsm.unlock_shared();
    rsm.unlock_shared();

    // we expect helper_fail to fail
    std::thread one(helper_fail);
    one.join();

    // relock
    rsm.lock();
    rsm.lock();
    rsm.lock_shared();
    rsm.lock_shared();

    // now try not unlocking shared enough times
    rsm.unlock();
    rsm.unlock();
    rsm.unlock();
    rsm.unlock_shared();

    // again we expect helper fail to fail
    std::thread two(helper_fail);
    two.join();

    // unlock the last shared
    rsm.unlock_shared();

    // helper pass should pass now
    std::thread three(helper_pass);
    three.join();
}

BOOST_AUTO_TEST_CASE(rsm_promotion_tests)
{
    rsm_lock_shared_while_exclusive_owner();
}

BOOST_AUTO_TEST_CASE(rsm_promotion_tests_perf)
{
    uint32_t run = 0;
    std::chrono::steady_clock::time_point startTime_1 = GetTimeNow();
    for (run = 0; run < 10000; ++run)
    {
        rsm_lock_shared_while_exclusive_owner();
    }
    std::chrono::steady_clock::time_point endTime_1 = GetTimeNow();
    int64_t duration_1 = GetDuration(startTime_1, endTime_1);
    printf("rsm_lock_shared_while_exclusive_owner took %" PRId64 " microseconds \n", duration_1);
}

BOOST_AUTO_TEST_SUITE_END()
