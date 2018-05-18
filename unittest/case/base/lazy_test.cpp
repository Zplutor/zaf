#include <gtest/gtest.h>
#include <memory>
#include <thread>
#include <zaf/base/lazy.h>
#include "test_object.h"

namespace {

std::size_t g_initializer_delete_count = 0;

struct Initializer {

    void operator()(TestObject& object) {
        object.value = "initialized";
    }

    ~Initializer() {
        ++g_initializer_delete_count;
    }
};

}


TEST(Lazy, DeleteInitializer) {

    //Set initializer before instantiating.
    {
        g_initializer_delete_count = 0;

        zaf::Lazy<TestObject> lazy;
        Initializer initializer;
        lazy.SetInitializer(initializer); //A copied initialized should be deleted.
        lazy.GetInstance();  //The inner initializer should be deleted after instantiating.

        ASSERT_EQ(g_initializer_delete_count, 2); //Total should be 2.
    }

    //Set initializer after instantiating.
    {
        g_initializer_delete_count = 0;

        zaf::Lazy<TestObject> lazy;
        Initializer initializer;
        lazy.GetInstance();
        lazy.SetInitializer(initializer); //A copied initialized should be deleted.
        
        ASSERT_EQ(g_initializer_delete_count, 1); //Total should be 1.
    }
}


TEST(Lazy, Instantiate) {

    {
        zaf::Lazy<TestObject> lazy;
        auto& object = lazy.GetInstance();
        ASSERT_TRUE(object.default_constructor_called);
        ASSERT_EQ(object.value, "");
    }

    {
        zaf::Lazy<TestObject> lazy;
        lazy.SetInitializer(Initializer());
        auto& object = lazy.GetInstance();
        ASSERT_TRUE(object.default_constructor_called);
        ASSERT_EQ(object.value, "initialized");
    }
}


TEST(Lazy, IsInitialized) {

    zaf::Lazy<TestObject> lazy;
    ASSERT_FALSE(lazy.IsInitialized());

    lazy.GetInstance();
    ASSERT_TRUE(lazy.IsInitialized());
}


/**
 Test instantiating a Lazy instance in multiple-thread context.

 Start several threads that access the same Lazy instance. The underlying
 instance should be instantiated only once.
 */
struct InstantiateInMultipleThreadsTester {

    struct TestObject {
        TestObject() {
            ++s_call_constructor_count;
            MakeThreadContextSwitch();
        }
        std::atomic<std::size_t> value;
    };

    static void MakeThreadContextSwitch() {
        for (std::size_t loop_count = 0; loop_count <= 10; ++loop_count) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    static bool Test() {
        
        s_call_constructor_count = 0;
        s_call_initializer_count = 0;

        zaf::Lazy<TestObject> lazy;
        lazy.SetInitializer([](TestObject&) {
            ++s_call_initializer_count;
            MakeThreadContextSwitch();
        });

        //Start threads.
        const std::size_t total_thread_count = 10;
        std::vector<std::shared_ptr<std::thread>> threads;
        for (std::size_t thread_count = 0; thread_count < total_thread_count; ++thread_count) {
            auto thread = std::make_shared<std::thread>([&lazy]() {
                lazy->value++;
            });
            threads.push_back(thread);
        }

        //Wait for all threads terminating.
        for (const auto& each_thread : threads) {
            if (each_thread->joinable()) {
                each_thread->join();
            }
        }

        if (lazy->value.load() != total_thread_count) {
            return false;
        }

        return (s_call_constructor_count == 1) && (s_call_initializer_count == 1);
    }

    static std::size_t s_call_constructor_count;
    static std::size_t s_call_initializer_count;
};

std::size_t InstantiateInMultipleThreadsTester::s_call_constructor_count = 0;
std::size_t InstantiateInMultipleThreadsTester::s_call_initializer_count = 0;

TEST(Lazy, InstantiateInMultipleThreads) {
    ASSERT_TRUE(InstantiateInMultipleThreadsTester::Test());
}