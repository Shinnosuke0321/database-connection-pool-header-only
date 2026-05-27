#pragma once
#include <atomic>
#include <condition_variable>
#include <chrono>
#include <mutex>
#include <queue>
#include <expected>
#include <semaphore>
#include <vector>
#include <thread>
#include <stop_token>
#include "connection.h"
#include "connection_factory.h"
#include "connection_manager.h"
#include <core/memory/intrusive_ptr.h>

namespace database {
    struct pool_config {
        std::size_t max_size = std::thread::hardware_concurrency();
        std::size_t init_size = 10;
        bool is_eager = false;
    };

    template<class T>
    requires std::derived_from<T, IConnection>
    class connection_pool: public core::ref_counted<connection_pool<T>> {
    public:
        static smart_ptr::intrusive_ptr<connection_pool> create(std::shared_ptr<ConnectionFactory> factory, const pool_config& opt = pool_config()) noexcept {
            return smart_ptr::intrusive_ptr<connection_pool>(new connection_pool(std::move(factory), std::move(opt)));
        }
    public:
        using SharedFactory = std::shared_ptr<ConnectionFactory>;
        using AcquireResult = std::expected<connection_manager<T>, connection_error>;

        AcquireResult acquire(std::chrono::microseconds timeout = std::chrono::microseconds{10000}) noexcept {
            std::unique_lock lk(m_mutex);
            if (!m_connections.empty()) {
                auto conn = std::move(m_connections.front());
                m_connections.pop();
                ++m_conn_in_use;
                lk.unlock();
                return wrap_connection(std::move(conn));
            }

            if (m_cv.wait_for(lk, timeout, [this] { return !m_connections.empty(); })) {
                auto conn = std::move(m_connections.front());
                m_connections.pop();
                lk.unlock();
                return wrap_connection(std::move(conn));
            }
            if (m_config.max_size >= ++m_conn_in_use + m_connections.size()) {
                lk.unlock();
                auto conn_res = m_factory->create_connection<T>();
                if (!conn_res) {
                    std::lock_guard lock(m_mutex);
                    --m_conn_in_use;
                    return std::unexpected(std::move(conn_res.error()));
                }
                return wrap_connection(std::move(conn_res.value()));
            }

            if (m_cv.wait_for(lk, timeout, [this] { return !m_connections.empty(); })) {
                auto conn = std::move(m_connections.front());
                m_connections.pop();
                ++m_conn_in_use;
                lk.unlock();
                return wrap_connection(std::move(conn));
            }
            using conn_err_types::Timeout;
            return MAKE_UNEXPECTED_ERROR(connection_error, Timeout, "Connection acquisition timed out");
        }
        void wait_for_warmup() noexcept {
            using namespace std::literals;
            while (!m_pool_ready.load(std::memory_order_acquire)) {
                m_pool_ready.wait(false, std::memory_order_acquire);
            }
            for (auto& t : m_threads) {
                if (t.joinable()) t.join();
            }
            m_threads.clear();
        }

        connection_pool() noexcept = delete;

        ~connection_pool() override {
            for (auto& t : m_threads) {
                t.request_stop();
                if (t.joinable()) t.join();
            }
        }
    private:
        explicit connection_pool(SharedFactory factory, const pool_config& opt = pool_config()) noexcept
        : m_config(opt),
          m_factory(std::move(factory))
        {
            if (m_config.is_eager && m_config.init_size > 0 && m_config.max_size > 0 && m_config.max_size >= m_config.init_size) {
                warmup_pool();
            }
            else {
                m_pool_ready.store(true, std::memory_order_release);
                m_pool_ready.notify_all();
            }
        }
        void warmup_pool() noexcept {
            using namespace std::literals;
            m_threads.reserve(m_config.init_size);
            for (uint32_t i = 0; i < m_config.init_size; ++i) {
                m_threads.emplace_back([this](const std::stop_token &st) { fill_pool(st);});
            }
        }
        void fill_pool(const std::stop_token& st) noexcept {
            using namespace std::chrono_literals;
            for (;;) {
                if (st.stop_requested())
                    return;
                {
                    std::unique_lock lk(m_mutex);
                    if (m_connections.size() + m_conn_in_use >= m_config.init_size)
                        return;
                }

                auto conn_res = m_factory->create_connection<T>();
                if (!conn_res) {
                    std::this_thread::sleep_for(1000ms);
                    continue;
                }

                bool is_ready = false;
                {
                    std::unique_lock lk(m_mutex);
                    m_connections.push(std::move(conn_res.value()));
                    is_ready = m_connections.size() == m_config.init_size;
                }
                if (is_ready) {
                    m_pool_ready.store(true, std::memory_order_release);
                    m_pool_ready.notify_all();
                }
                return;
            }
        }
        connection_manager<T> wrap_connection(std::unique_ptr<T> c) noexcept {
            auto instance = this->intrusive_from_this();
            auto releaser = [instance](std::unique_ptr<T> returned_conn) noexcept {
                if (!returned_conn)
                    return;
                {
                    std::unique_lock<std::mutex> lk(instance->m_mutex);
                    instance->m_connections.push(std::move(returned_conn));
                    --instance->m_conn_in_use;
                }
                instance->m_cv.notify_one();
            };

            return connection_manager<T>(std::move(c), std::move(releaser));
        }

    private:
        mutable std::mutex m_mutex;
        std::atomic_bool m_pool_ready = false;
        pool_config m_config;
        uint32_t m_conn_in_use = 0;
        std::queue<std::unique_ptr<T>> m_connections;
        std::condition_variable m_cv;
        std::shared_ptr<ConnectionFactory> m_factory;
        std::vector<std::jthread> m_threads;
    };
} // namespace Core::Database
