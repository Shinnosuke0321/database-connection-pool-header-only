//
// Created by Shinnosuke Kawai on 3/11/26.
//
#pragma once
#include <string>

namespace core {
    class error_base {
    public:
        virtual ~error_base() = default;
        virtual std::string to_str() const noexcept = 0;
    };

    class connection_error: public error_base {
    public:
        enum class type {
            ConnectionFailed, MissingConfig, FactoryNotRegistered, Timeout, SocketFailed, AuthFailed
        };
        static connection_error ConnectionFailed(const char* str) noexcept {
            return connection_error{type::ConnectionFailed, str};
        }
        static connection_error MissingConfig(const char* str) noexcept {
            return connection_error{type::MissingConfig, str};
        }
        static connection_error FactoryNotRegistered(const char* str) noexcept {
            return connection_error{type::FactoryNotRegistered, str};
        }
        static connection_error Timeout(const char* str) noexcept {
            return connection_error{type::Timeout, str};
        }
        static connection_error SocketFailed(const char* str) noexcept {
            return connection_error{type::SocketFailed, str};
        }
        static connection_error AuthFailed(const char* str) noexcept {
            return connection_error{type::AuthFailed, str};
        }

        std::string to_str() const noexcept override {
            return std::format("ConnectionError [{}]: {}", type_str(), m_message);
        }

        type get_code() const noexcept { return m_type; }
    private:
        connection_error(const type type, const char* message) : m_type(type), m_message(message) {}
        connection_error(const type type, std::string&& message) : m_type(type), m_message(std::move(message)) {}

        std::string type_str() const noexcept {
            switch (m_type) {
                case type::ConnectionFailed: return "ConnectionFailed";
                case type::MissingConfig: return "MissingConfig";
                case type::FactoryNotRegistered: return "FactoryNotRegistered";
                case type::Timeout: return "Timeout";
                case type::SocketFailed: return "SocketFailed";
                case type::AuthFailed: return "AuthFailed";
            }
            return "Unknown";
        }
    private:
        type m_type;
        std::string m_message{};
    };

}