//
// Created by Shinnosuke Kawai on 5/13/26.
//

#pragma once
#include <memory>
#include <type_traits>
#include <utility>
#include <functional>

namespace core::memory {

    // Small buffer optimization size (adjust as needed)
    constexpr std::size_t SBO_SIZE = 32;
    constexpr std::size_t SBO_ALIGN = alignof(std::max_align_t);

    template<class>
    class unique_function;

    template<class return_t, class... Args>
    class unique_function<return_t(Args...)> {
        struct concept_t {
            virtual ~concept_t() = default;
            virtual return_t invoke(Args... args) = 0;
            virtual concept_t* move_into(void* buffer) noexcept = 0;
            virtual void destroy() noexcept = 0;
        };

        template<typename func_t>
        struct model final : concept_t {
            func_t callable;

            explicit model(func_t&& f) : callable(std::forward<func_t>(f)) {}

            model(const model&) = delete;
            model& operator=(const model&) = delete;

            model(model&&) noexcept = default;
            model& operator=(model&&) noexcept = default;

            return_t invoke(Args... args) override {
                if constexpr (std::is_void_v<return_t>) {
                    std::invoke(callable, std::forward<Args>(args)...);
                    return;
                } else {
                    return std::invoke(callable, std::forward<Args>(args)...);
                }
            }

            concept_t* move_into(void* buffer) noexcept override {
                return new (buffer) model(std::move(callable));
            }

            void destroy() noexcept override {
                this->~model();
            }

            ~model() override = default;
        };

        // Determine if we should use SBO for this type
        template<typename T>
        static constexpr bool use_sbo_v =
            sizeof(model<T>) <= SBO_SIZE &&
            alignof(model<T>) <= SBO_ALIGN &&
            std::is_nothrow_move_constructible_v<T>;
    public:
        unique_function() noexcept = default;
        unique_function(std::nullptr_t) noexcept {}

        unique_function(const unique_function&) = delete;
        unique_function& operator=(const unique_function&) = delete;

        unique_function(unique_function&& other) noexcept: m_use_sbo(other.m_use_sbo) {
            if (other.m_use_sbo && other.m_ptr) {
                // Move from SBO buffer
                auto* src = reinterpret_cast<concept_t*>(other.m_storage.buffer);
                m_ptr = src->move_into(m_storage.buffer);
                src->destroy();
                other.m_use_sbo = false;
            } else {
                // Move heap pointer
                m_storage.heap_ptr = other.m_storage.heap_ptr;
                other.m_storage.heap_ptr = nullptr;
                m_ptr = other.m_ptr;
            }
            other.m_ptr = nullptr;
        }

        unique_function& operator=(unique_function&& other) noexcept {
            if (this != &other) {
                reset();
                if (other.m_use_sbo && other.m_ptr) {
                    auto* src = reinterpret_cast<concept_t*>(other.m_storage.buffer);
                    m_ptr = src->move_into(m_storage.buffer);
                    src->destroy();
                    other.m_use_sbo = false;
                    m_use_sbo = true;
                } else {
                    m_storage.heap_ptr = other.m_storage.heap_ptr;
                    other.m_storage.heap_ptr = nullptr;
                    m_use_sbo = false;
                    m_ptr = other.m_ptr;
                }
                other.m_ptr = nullptr;
            }
            return *this;
        }

        template<
            typename func_t,
            typename decayed_t = std::decay_t<func_t>,
            typename = std::enable_if_t<
                !std::is_same_v<decayed_t, unique_function> &&
                !std::is_same_v<decayed_t, std::nullptr_t> &&
                std::is_invocable_r_v<return_t, decayed_t&, Args...>
            >
        >
        unique_function(func_t&& f) {
            using model_t = model<decayed_t>;

            if constexpr (use_sbo_v<decayed_t>) {
                // This checks if sizeof(model_t) <= SBO_SIZE
                std::println("Using SBO - model size: {} bytes", sizeof(model_t));
                m_ptr = new (m_storage.buffer) model_t(std::forward<func_t>(f));
                m_use_sbo = true;
            } else {
                std::println("Using heap - model size: {} bytes", sizeof(model_t));
                m_ptr = new model_t(std::forward<func_t>(f));
                m_storage.heap_ptr = m_ptr;
                m_use_sbo = false;
            }
        }

        template<
            typename func_t,
            typename decayed_t = std::decay_t<func_t>,
            typename = std::enable_if_t<
                !std::is_same_v<decayed_t, unique_function> &&
                !std::is_same_v<decayed_t, std::nullptr_t> &&
                std::is_invocable_r_v<return_t, decayed_t&, Args...>
            >
        >
        unique_function& operator=(func_t&& f) {
            reset();

            using model_t = model<decayed_t>;

            if constexpr (use_sbo_v<decayed_t>) {
                m_ptr = new (m_storage.buffer) model_t(std::forward<func_t>(f));
                m_use_sbo = true;
            } else {
                m_ptr = new model_t(std::forward<func_t>(f));
                m_storage.heap_ptr = m_ptr;
                m_use_sbo = false;
            }
            return *this;
        }

        unique_function& operator=(std::nullptr_t) noexcept {
            reset();
            return *this;
        }

        // Non-const lvalue ref qualifier
        return_t operator()(Args... args) & {
            if (!m_ptr) {
                throw std::bad_function_call();
            }
            if constexpr (std::is_void_v<return_t>) {
                m_ptr->invoke(std::forward<Args>(args)...);
                return;
            } else {
                return m_ptr->invoke(std::forward<Args>(args)...);
            }
        }

        return_t operator()(Args... args) const& {
            if (!m_ptr) {
                throw std::bad_function_call();
            }
            if constexpr (std::is_void_v<return_t>) {
                m_ptr->invoke(std::forward<Args>(args)...);
                return;
            } else {
                return m_ptr->invoke(std::forward<Args>(args)...);
            }
        }

        return_t operator()(Args... args) && {
            if (!m_ptr) {
                throw std::bad_function_call();
            }
            if constexpr (std::is_void_v<return_t>) {
                m_ptr->invoke(std::forward<Args>(args)...);
                return;
            } else {
                return m_ptr->invoke(std::forward<Args>(args)...);
            }
        }

        explicit operator bool() const noexcept {
            return m_ptr != nullptr;
        }

        bool operator==(std::nullptr_t) const noexcept {
            return !m_ptr;
        }

        bool operator==(const unique_function& other) const noexcept {
            return (m_ptr == nullptr) && (other.m_ptr == nullptr);
        }

        void swap(unique_function& other) noexcept {
            storage tmp = m_storage;
            bool tmp_sbo = m_use_sbo;
            concept_t* tmp_ptr = m_ptr;

            m_storage = other.m_storage;
            m_use_sbo = other.m_use_sbo;
            m_ptr = other.m_ptr;

            if (m_use_sbo && m_ptr) {
                m_ptr = reinterpret_cast<concept_t *>(m_storage.buffer);
            }

            other.m_storage = tmp;
            other.m_use_sbo = tmp_sbo;
            other.m_ptr = tmp_ptr;
            if (other.m_use_sbo && other.m_ptr) {
                other.m_ptr = reinterpret_cast<concept_t *>(other.m_storage.buffer);
            }
        }

        ~unique_function() {
            reset();
        }

    private:
        void reset() noexcept {
            if (m_ptr) {
                if (m_use_sbo) {
                    m_ptr->destroy();
                } else {
                    delete m_ptr;
                }
                m_ptr = nullptr;
                m_use_sbo = false;
            }
        }

        union storage {
            void* heap_ptr;
            std::byte buffer[SBO_SIZE];
        };

        storage m_storage{};
        concept_t* m_ptr = nullptr;
        bool m_use_sbo = false;
    };
}