/**
 * @file 01_thread.cpp
 * @author Ferdinand Tonby-Strandborg
 * @version 0.0.1-alpha
 * @date 2025-11-5
 * @copyright Copyright (c) 2025 Ferdinand Tonby-Strandborg. Licensed under the MIT license.
 * 
 * Tests for utilities and functions related to class `Thread` from `simply-threading` 
 */
#include <simply/threading.h>

#include "gtest/gtest.h"

#include <atomic>
#include <chrono>
#include <system_error>

#if SIMPLY_std20plus
    #include <stop_token>
#endif

using namespace simply;

// Will need several priority tests...
enum TestPriority {
    LOW,
    HIGH,
    V_HIGH,
    REALTIME
};

// ========
// >> Id 
// ========
TEST(Id, Null) {
    // Default constructed id must not equal std::this_thread::id
    ASSERT_NE(Thread::id(), this_thread::get_id());
    ASSERT_EQ(Thread::id(), Thread::id());
}

// ===========
// >> Thread
// ===========
TEST(Thread, Null) {
    // Default constructed Thread must:
    Thread thread;

    // 1. Not be callable
    EXPECT_FALSE(thread.joinable());

    // 2. Must throw on all operations
    EXPECT_THROW(thread.join(), std::system_error);
    EXPECT_THROW(thread.join(5), std::system_error);
    EXPECT_THROW(thread.join_for(std::chrono::milliseconds(5)), std::system_error);
    EXPECT_THROW(thread.join_until(std::chrono::high_resolution_clock::now()), std::system_error);
    EXPECT_THROW(thread.detach(), std::system_error);

    // 3. Must have id equal to default
    EXPECT_EQ(thread.get_id(), Thread::id());

    // 4. Check hardware_concurrency
    EXPECT_GT(thread.hardware_concurrency(), 0);
}

TEST(Thread, Move) {
    Thread t1([](){});
    Thread t2([](){});

    Thread::id id1 = t1.get_id();
    Thread::id id2 = t2.get_id();

    t1.swap(t2);
    ASSERT_EQ(t1.get_id(), id2);
    ASSERT_EQ(t2.get_id(), id1);

    std::swap(t1, t2);
    ASSERT_EQ(t1.get_id(), id1);
    ASSERT_EQ(t2.get_id(), id2);

    ASSERT_TRUE(t1.joinable() && t2.joinable());
    
    // Move assignment
    t1 = std::move(t2);
    ASSERT_FALSE(t2.joinable());
    ASSERT_TRUE(t1.joinable());
    ASSERT_EQ(t2.get_id(), Thread::id());
    ASSERT_EQ(t1.get_id(), id2);

    // Move constructor
    Thread t3(std::move(t1));
    ASSERT_FALSE(t1.joinable());
    ASSERT_TRUE(t3.joinable());
    ASSERT_EQ(t1.get_id(), Thread::id());
    ASSERT_EQ(t3.get_id(), id2);
}

#if SIMPLY_WINDOWS
// TEST(Thread, WindowsOptions) {
//     Thread null_thread;
//     EXPECT_THROW(null_thread.set_priority(Thread::Priority::HIGH), std::system_error);
//     EXPECT_THROW(null_thread.get_priority(), std::system_error);
//     EXPECT_THROW(null_thread.set_name("Test"), std::system_error);
//     EXPECT_THROW(null_thread.get_name(), std::system_error);
    
//     Thread thread([](){ this_thread::sleep(10); });
//     ASSERT_NO_THROW(thread.set_priority(Thread::Priority::HIGH));
//     EXPECT_EQ(thread.get_priority(), Thread::Priority::HIGH);
//     ASSERT_NO_THROW(thread.set_name("Test"));
//     EXPECT_EQ(thread.get_name(), "Test");
// }

#else
TEST(Thread, LinuxOptions) {
    /// Todo...
}

#endif

// These are a little more finicky as the times don't guarantee order,
// just "encourage" order of execution
TEST(Thread, Timing) {
    
}

#if SIMPLY_std20plus
    TEST(Thread, StopToken) {
        bool stopped = false;
        auto stoppable = [&stopped](std::stop_token token){
            while ( !token.stop_requested() )
                this_thread::sleep(10);
            stopped = true;
        };

        Thread t1(stoppable);
        std::stop_source s = t1.get_stop_source();
        std::stop_token  t = t1.get_stop_token();
        s.request_stop();
        this_thread::sleep(50);
        EXPECT_TRUE(stopped);
        EXPECT_TRUE(t.stop_requested());
        EXPECT_TRUE(t1.joinable());
        stopped = false;

        Thread t2(stoppable);
        t2.join();
        EXPECT_TRUE(stopped);
    }
#endif

// ======================
// Thread >> Callables
// ======================
void no_return() {}

int simple_return() { return 0; }

static bool global_flag;
bool global_return() { return (global_flag = true); }

int simple_arg(int val) { return val; }

TEST(Thread, Callables) {
    auto no_return_lambda = [](){};
    EXPECT_NO_THROW(Thread(no_return_lambda));

    auto return_lambda = [](){ return 5; };
    EXPECT_NO_THROW(Thread(return_lambda));

    auto no_return_param_lambda = []( double v ){};
    EXPECT_NO_THROW(Thread(no_return_param_lambda, 5));

    auto return_param_lambda = []( double v ) { return v; };
    EXPECT_NO_THROW(Thread(return_param_lambda, 5));

    bool flag = false;
    auto local_lambda = [&flag](){ return (flag=true); };
    EXPECT_NO_THROW(Thread(local_lambda));
    EXPECT_TRUE(flag);

    EXPECT_NO_THROW(Thread(no_return));

    EXPECT_NO_THROW(Thread(simple_return));

    global_flag = false;
    EXPECT_NO_THROW(Thread(global_flag));
    EXPECT_TRUE(global_flag);

    EXPECT_NO_THROW(Thread(simple_arg, 1));
}