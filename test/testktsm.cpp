#include "catch2/catch_amalgamated.hpp"

#include <qsharedmemory.h>

#include <thread>
#include <chrono>

TEST_CASE("Create, destroy attach and detach tests", "[init]") {
    SECTION("Create and destroy") {
        QSharedMemory sm("test_key");
        CHECK(sm.key() == "test_key");
        CHECK_THAT(sm.nativeKey(), Catch::Matchers::ContainsSubstring("testkey"));
    }

    SECTION("Attach and detach") {
        QSharedMemory sm_c("test_key"), sm_w("test_key"), sm_r("test_key");
        REQUIRE_FALSE(sm_c.isAttached());
        REQUIRE_FALSE(sm_c.attach(QSharedMemory::ReadOnly));
        REQUIRE_FALSE(sm_w.isAttached());

        REQUIRE(sm_c.create(256));
        REQUIRE(sm_c.isAttached());
        REQUIRE(sm_w.attach(QSharedMemory::ReadWrite));
        REQUIRE(sm_w.isAttached());
        REQUIRE(sm_r.attach(QSharedMemory::ReadOnly));
        REQUIRE(sm_r.isAttached());

        REQUIRE(sm_c.detach());
        REQUIRE_FALSE(sm_c.isAttached());
        REQUIRE(sm_w.isAttached());
        REQUIRE(sm_r.isAttached());

        REQUIRE(sm_w.detach());
        REQUIRE_FALSE(sm_w.isAttached());
        REQUIRE(sm_r.detach());
        REQUIRE_FALSE(sm_r.isAttached());

        REQUIRE_FALSE(sm_r.attach(QSharedMemory::ReadOnly));
    }
}

TEST_CASE("Lock and unlock tests", "[lock]") {
    QSharedMemory sm_c("test_key"), sm_w("test_key");
    REQUIRE(sm_c.create(256));
    REQUIRE(sm_w.attach(QSharedMemory::ReadWrite));

    REQUIRE(sm_c.lock());
    REQUIRE(sm_c.lock());
    std::thread t([&sm_c]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        REQUIRE(sm_c.unlock());
    });
    t.join();
    REQUIRE(sm_w.lock());
    REQUIRE_FALSE(sm_c.unlock());
    REQUIRE(sm_w.lock());
    REQUIRE(sm_w.unlock());
}

TEST_CASE("Transfer data tests", "[sharemem]") {
    QSharedMemory sm_c("test_key"), sm_w("test_key"), sm_r("test_key");
    REQUIRE(sm_c.create(256));
    REQUIRE(sm_w.attach(QSharedMemory::ReadWrite));
    REQUIRE(sm_r.attach(QSharedMemory::ReadOnly));

    const char *data = "Hello world from KTSM QSharedMemory!";

    REQUIRE(sm_w.lock());
    memcpy(sm_w.data(), data, strlen(data) + 1);
    REQUIRE(sm_w.unlock());
    REQUIRE(sm_r.lock());
    REQUIRE_THAT((const char *)sm_r.data(), Catch::Matchers::Equals(data));
    REQUIRE(sm_r.unlock());
    REQUIRE(sm_w.lock());
    REQUIRE_THAT((const char *)sm_w.data(), Catch::Matchers::Equals(data));
    memcpy(sm_w.data(), "HELLO", 5);
    REQUIRE(sm_w.unlock());
    REQUIRE(sm_c.lock());
    REQUIRE_THAT((const char *)sm_c.data(), Catch::Matchers::StartsWith("HELLO"));
    REQUIRE(sm_c.unlock());
    REQUIRE(sm_r.lock());
    REQUIRE_THAT((const char *)sm_r.data(), Catch::Matchers::StartsWith("HELLO"));
    REQUIRE(sm_r.unlock());
}

