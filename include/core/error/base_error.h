//
// Created by Shinnosuke Kawai on 3/11/26.
//
#pragma once
#include <string>
#include <format>
#include <string_view>
#include <concepts>

namespace core {
    class error_base {
    public:
        virtual ~error_base() = default;

        virtual std::string_view message() const noexcept = 0;
        virtual std::string_view category() const noexcept = 0;
        virtual std::string_view code_string() const noexcept = 0;
        virtual std::string to_string() const noexcept = 0;
    };

    template<typename T, typename Enum>
    concept ErrorTraits = requires(Enum e) {
        { T::category_name() } -> std::same_as<std::string_view>;
        { T::code_to_string(e) } -> std::same_as<std::string_view>;
    };

    template<typename Derived, typename Enum>
    class typed_error : public error_base {
        static void _check() {
            static_assert(ErrorTraits<Derived, Enum>,
                "Derived must implement:\n"
                "  static std::string_view category_name()\n"
                "  static std::string_view code_to_string(Enum)");
        }
    public:
        using code_type = Enum;

        typed_error(Enum code, std::string msg): m_code(code), m_message(std::move(msg)) {
            _check();
        }

        std::string_view message() const noexcept override {
            return m_message;
        }

        Enum get_code() const noexcept {
            return m_code;
        }

        std::string_view category() const noexcept override {
            return Derived::category_name();
        }

        std::string_view code_string() const noexcept override {
            return Derived::code_to_string(m_code);
        }

        std::string to_string() const noexcept override {
            return std::format("[{}:{}] {}",
                category(),
                code_string(),
                message()
            );
        }
        ~typed_error() override = default;

    protected:
        Enum m_code;
        std::string m_message;
    };

#define ERROR_CATEGORY_NAME(name) \
    using base = typed_error; \
    using base::base; \
    static std::string_view category_name() noexcept { return #name;}
}