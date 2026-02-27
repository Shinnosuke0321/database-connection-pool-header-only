//
// Created by Shinnosuke Kawai on 2/24/26.
//
#include <cassert>
#include <chrono>
#include <memory>
#include <gtest/gtest.h>
#include <database/connection_pool.h>

using namespace std::chrono_literals;

namespace {
    struct FakeConn : Core::Database::IConnection {
        int value = 42;
        ~FakeConn() override {
            assert(value == 42);
        };
    };
}

TEST(ConnectionPoolTest, AsynTest) {
    auto factory = std::make_shared<Core::Database::ConnectionFactory>();

    factory->register_factory<FakeConn>([]() -> Core::Database::ConnectionResult {
        return std::unique_ptr<Core::Database::IConnection>(new FakeConn{});
    });

    Core::Database::ConnectionPool<FakeConn>::PoolConfig cfg;
    cfg.init_size = 1;
    cfg.max_size = 1;
    cfg.is_eager = true;

    auto pool = std_ex::make_intrusive<Core::Database::ConnectionPool<FakeConn>>(factory, cfg);
    pool->wait_for_warmup();

    {
        auto res = pool->acquire();
        ASSERT_TRUE(res.has_value());
        auto mgr = std::move(res.value());
        ASSERT_EQ(mgr->value,  42);
    } // manager destructor should return connection to pool

    auto res2 = pool->acquire(1s);
    ASSERT_TRUE(res2.has_value());
}
int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}