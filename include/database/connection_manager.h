//
// Created by Shinnosuke Kawai on 10/22/25.
//

#pragma once
#include "connection.h"
#include <functional>

namespace database {
    template<class T>
    requires std::derived_from<T, IConnection>
    class connection_manager {
    public:
        using releaser_t = std::function<void(std::unique_ptr<T>)>;

        explicit connection_manager(std::unique_ptr<T> connection, releaser_t&& releaser)
            : m_connection(std::move(connection)), m_releaser(std::move(releaser)) {}

        connection_manager(connection_manager&& other) noexcept
        : m_connection(std::move(other.m_connection)),
          m_releaser(std::move(other.m_releaser)) {
            other.m_releaser = nullptr;
        }
        connection_manager& operator=(connection_manager&& other) noexcept {
            if (this == &other) {
                return *this;
            }
            m_connection = std::move(other.m_connection);
            m_releaser = std::move(other.m_releaser);
            other.m_releaser = nullptr;
            return *this;
        };

        connection_manager(connection_manager const&) = delete;
        connection_manager& operator=(connection_manager const&) = delete;

        ~connection_manager() {
            release();
        }

        T& operator*() { return *m_connection; }
        const T& operator*() const { return *m_connection; }
        T* operator->() { return m_connection.get();}
        const T* operator->() const { return m_connection.get();}
    private:
        void release() noexcept {
            if (m_connection && m_releaser)
                m_releaser(std::move(m_connection));
            else
                m_connection.reset();
        }
    private:
        std::unique_ptr<T> m_connection;
        releaser_t m_releaser;
    };
}
