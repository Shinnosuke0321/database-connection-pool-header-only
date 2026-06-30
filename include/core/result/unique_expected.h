//
// Created by Shinnosuke Kawai on 6/15/26.
//
#pragma once
#include <concepts>
#include <type_traits>

namespace core::result {
    template<class success_t, class failure_t>
    concept is_moveable = std::is_move_constructible_v<success_t> &&
                          std::is_move_assignable_v<success_t> &&
                          std::is_move_constructible_v<failure_t> &&
                          std::is_move_assignable_v<failure_t>;

    template<class success_t, class failure_t>
    concept is_copyable = std::is_copy_constructible_v<success_t> &&
                          std::is_copy_assignable_v<success_t> &&
                          std::is_copy_constructible_v<failure_t> &&
                          std::is_copy_assignable_v<failure_t>;

    template<class E>
    class oops {
    public:
        constexpr explicit oops() = default;
        constexpr explicit oops(E&& error) noexcept: m_error(std::forward<E>(error)) {}
    private:
        E m_error;
    };

    template<class success_t, class failure_t>
    requires (is_moveable<success_t, failure_t> || !is_copyable<success_t, failure_t>)
    class unique_expected {
    public:
        unique_expected() = default;

        explicit constexpr unique_expected(success_t value): m_storage(std::move(value)) {}
        unique_expected(const unique_expected&) = delete;
        unique_expected& operator=(const unique_expected&) = delete;

        unique_expected(unique_expected&& other) noexcept: m_storage(std::move(other.m_storage)) {};
        unique_expected& operator=(unique_expected&& other) noexcept {
            if (this == &other)
                return *this;
            m_storage = std::move(other.m_storage);
            return *this;
        };


        bool has_value() const noexcept { return m_storage.m_has_value; }

        ~unique_expected() noexcept { m_storage.~storage_t(); }
    private:
        struct storage_t {

            storage_t() = default;

            explicit storage_t(success_t&& value): success(std::move(value)), m_has_value(true) {}
            ~storage_t() {
                destroy();
            };
            storage_t(const storage_t&) = delete;
            storage_t& operator=(const storage_t&) = delete;

            storage_t(storage_t&& other) noexcept: m_has_value(other.m_has_value) {
                if (m_has_value) {
                    success = new (&other.success) success_t(std::move(other.success));
                } else {
                    failure = new (&other.failure) failure_t(std::move(other.failure));
                }
                // should I destroy src object?
                other.destroy();
            }
            storage_t& operator=(storage_t&& other) noexcept {
                if (this == &other)
                    return *this;
                if (other.m_has_value) {
                    m_has_value = true;
                    success = new (&success) success_t(std::move(other.success));
                } else {
                    m_has_value = false;
                    failure = new (&failure) failure_t(std::move(other.failure));
                }
                other.destroy();
                return *this;
            }

            void destroy() {
                if (m_has_value) {
                    success.~success_t();
                } else {
                    failure.~failure_t();
                }
            }
            union {
                success_t success;
                failure_t failure;
            };
            bool m_has_value{false};
        };
        storage_t m_storage{};
    };
}