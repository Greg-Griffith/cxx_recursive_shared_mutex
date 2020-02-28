// Copyright (c) 2019 Greg Griffith
// Copyright (c) 2019 The Bitcoin Unlimited developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "promotable_recursive_shared_mutex.h"
#include "test/test_cxx_rsm.h"
#include "test/timer.h"

#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(promotable_starvation_tests, TestSetup)

class rsm_watcher : public promotable_recursive_shared_mutex
{
public:
    size_t get_shared_owners_count() { return _read_owner_ids.size(); }
};

rsm_watcher rsm;
std::vector<int> rsm_guarded_vector;

std::atomic<int> shared_locks{0};
std::atomic<int> exclusive_locks{0};
std::atomic<int> rejected{0};

bool shared_only_starved_last()
{
    return rsm.try_lock_shared();
}

void shared_only_starved()
{
    bool result = rsm.try_lock_shared();
    // shoud always be false
    if (result == false)
    {
        rejected++;
    }
    // we dont need to unlock because it never successfully locked
}

void shared_only()
{
    rsm.lock_shared();
    shared_locks++;
    while (rejected != 3) ;
    rsm.unlock_shared();
}

void exclusive_only()
{
    while(shared_locks != 3) ;
    exclusive_locks++;
    rsm.lock();
    rsm_guarded_vector.push_back(4);
    rsm.unlock();
}

void promoting_thread()
{
    rsm.lock_shared();
    shared_locks++;
    while(exclusive_locks != 1) ;
    bool promoted = rsm.try_promotion();
    BOOST_CHECK_EQUAL(promoted, true);
    rsm_guarded_vector.push_back(7);
    rsm.unlock();
    rsm.unlock_shared();
}

/*
 * if a thread askes for a promotion while no other thread
 * is currently asking for a promotion it will be put in line to grab the next
 * exclusive lock even if another threads are waiting using lock()
 *
 * This test covers blocking of additional shared ownership aquisitions while
 * a thread is waiting for promotion.
 *
 */

void rsm_test_starvation()
{
    // clear the data vector at test start
    rsm_guarded_vector.clear();
    shared_locks = 0;
    exclusive_locks = 0;
    rejected = 0;

    // start up intial shared thread to block immidiate exclusive grabbing
    std::thread one(shared_only);
    std::thread two(shared_only);
    std::thread three(promoting_thread);
    std::thread four(exclusive_only);
    while (exclusive_locks != 1) ;
    // we should always get 3 because five, six, and seven should be blocked by
    // three promotion request leaving only one, two, and three with shared ownership
    BOOST_CHECK_EQUAL(rsm.get_shared_owners_count(), 3);
    std::thread five(shared_only_starved);
    BOOST_CHECK_EQUAL(rsm.get_shared_owners_count(), 3);
    std::thread six(shared_only_starved);
    BOOST_CHECK_EQUAL(rsm.get_shared_owners_count(), 3);
    bool result = shared_only_starved_last();
    BOOST_CHECK_EQUAL(rsm.get_shared_owners_count(), 3);
    if (result == false)
    {
        rejected++;
    }

    one.join();
    two.join();
    three.join();
    four.join();
    five.join();
    six.join();

    // 7 was added by the promoted thread, it should appear first in the vector
    rsm.lock_shared();
    BOOST_CHECK_EQUAL(7, rsm_guarded_vector[0]);
    BOOST_CHECK_EQUAL(4, rsm_guarded_vector[1]);
    rsm.unlock_shared();
}

BOOST_AUTO_TEST_CASE(rsm_starvation_test)
{
    rsm_test_starvation();
}

BOOST_AUTO_TEST_CASE(rsm_test_starvation_perf)
{
    uint32_t run = 0;
    std::chrono::steady_clock::time_point startTime_1 = GetTimeNow();
    for (run = 0; run < 10000; ++run)
    {
        rsm_test_starvation();
    }
    std::chrono::steady_clock::time_point endTime_1 = GetTimeNow();
    int64_t duration_1 = GetDuration(startTime_1, endTime_1);
    printf("PRO test_starvation took %" PRId64 " microseconds \n", duration_1);
}

BOOST_AUTO_TEST_SUITE_END()
