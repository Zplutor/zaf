#include <gtest/gtest.h>
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

TEST(Lazy, Size) {

    typedef std::uintptr_t InstanceType;

    ASSERT_EQ(sizeof(zaf::Lazy<InstanceType, zaf::LazyLock::Spin>), sizeof(InstanceType) + sizeof(std::uintptr_t));
    ASSERT_EQ(sizeof(zaf::Lazy<InstanceType, zaf::LazyLock::Mutex>), sizeof(InstanceType) + sizeof(std::uintptr_t) + sizeof(std::mutex));
}


template<zaf::LazyLock LockType>
struct DeleteInitializerWithoutInstantiateTester {
    static bool Test() {

        g_initializer_delete_count = 0;

        Initializer initializer;
        {
            zaf::Lazy<TestObject, LockType> lazy;
            lazy.SetInitializer(initializer);
        }

        //A copied initializer passed as argument is deleted.
        //A moved initializer is deleted while destroy lazy.
        //Total is twice.
        return (g_initializer_delete_count == 2);
    }
};

TEST(Lazy, DeleteInitializerWithoutInstantiate) {
    ASSERT_TRUE(DeleteInitializerWithoutInstantiateTester<zaf::LazyLock::Spin>::Test());
    ASSERT_TRUE(DeleteInitializerWithoutInstantiateTester<zaf::LazyLock::Mutex>::Test());
}


template<zaf::LazyLock LockType> 
struct InstantiateWithoutInitializerTester {
    static bool Test() {
        zaf::Lazy<TestObject, LockType> lazy;
        auto& object = lazy.GetInstance();
        return object.default_constructor_called;
    }
};

TEST(Lazy, InstantiateWithoutInitializer) {

    ASSERT_TRUE(InstantiateWithoutInitializerTester<zaf::LazyLock::Spin>::Test());
    ASSERT_TRUE(InstantiateWithoutInitializerTester<zaf::LazyLock::Mutex>::Test());
}


template<zaf::LazyLock LockType>
struct InstantiateWithInitializerTester {

    static bool Test() {

        g_initializer_delete_count = 0;

        zaf::Lazy<TestObject, LockType> lazy;
        Initializer initializer;
        lazy.SetInitializer(initializer);
        auto& object = lazy.GetInstance();

        if (! object.default_constructor_called) {
            return false;
        }

        if (object.value != "initialized") {
            return false;
        }
        
        //A copied initializer passed as argument is deleted.
        //A moved initializer is deleted after creating instance.
        //Total is twice.
        return g_initializer_delete_count == 2;
    }
};

TEST(Lazy, InstantiateWithInitializer) {

    ASSERT_TRUE(InstantiateWithInitializerTester<zaf::LazyLock::Spin>::Test());
    ASSERT_TRUE(InstantiateWithInitializerTester<zaf::LazyLock::Mutex>::Test());
}


template<zaf::LazyLock LockType>
struct IsInitializedTester {
    static bool Test() {

        auto test = [](bool set_initializer, std::function<void(TestObject&)> initializer) {
        
            zaf::Lazy<TestObject, LockType> lazy;
            if (set_initializer) {
                lazy.SetInitializer(initializer);
            }

            if (lazy.IsInitialized()) {
                return false;
            }

            lazy.GetInstance();
            return lazy.IsInitialized();
        };

        return 
            test(false, nullptr) && 
            test(true, nullptr) &&
            test(true, [](TestObject&) { });
    }
};

TEST(Lazy, IsInitialized) {

    ASSERT_TRUE(IsInitializedTester<zaf::LazyLock::Spin>::Test());
    ASSERT_TRUE(IsInitializedTester<zaf::LazyLock::Mutex>::Test());
}


/**
 Test instantiating a Lazy instance in multiple-thread context.

 Start several threads that access the same Lazy instance. The underlying
 instance should be instantiated only once.
 */
template<zaf::LazyLock LockType>
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

template<zaf::LazyLock LockType>
std::size_t InstantiateInMultipleThreadsTester<LockType>::s_call_constructor_count = 0;

template<zaf::LazyLock LockType>
std::size_t InstantiateInMultipleThreadsTester<LockType>::s_call_initializer_count = 0;

TEST(Lazy, InstantiateInMultipleThreads) {

    ASSERT_TRUE(InstantiateInMultipleThreadsTester<zaf::LazyLock::Spin>::Test());
    ASSERT_TRUE(InstantiateInMultipleThreadsTester<zaf::LazyLock::Mutex>::Test());
}