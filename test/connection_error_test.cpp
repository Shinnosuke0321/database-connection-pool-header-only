//
// Unit tests for Core::Database::ConnectionError
//
#include <gtest/gtest.h>
#include <database/connection.h>
#include <string>

using namespace core::database;

TEST(ConnectionErrorTest, AllSixFactoryMethods_ReturnCorrectCode) {
    ASSERT_EQ(core::connection_error::ConnectionFailed("msg").get_code(),    core::connection_error::type::ConnectionFailed);
    ASSERT_EQ(core::connection_error::MissingConfig("msg").get_code(),       core::connection_error::type::MissingConfig);
    ASSERT_EQ(core::connection_error::FactoryNotRegistered("msg").get_code(),core::connection_error::type::FactoryNotRegistered);
    ASSERT_EQ(core::connection_error::Timeout("msg").get_code(),             core::connection_error::type::Timeout);
    ASSERT_EQ(core::connection_error::SocketFailed("msg").get_code(),        core::connection_error::type::SocketFailed);
    ASSERT_EQ(core::connection_error::AuthFailed("msg").get_code(),          core::connection_error::type::AuthFailed);
}

TEST(ConnectionErrorTest, ToStr_ContainsTypeAndMessage) {
    {
        auto err = core::connection_error::ConnectionFailed("conn fail msg");
        auto s = err.to_str();
        ASSERT_NE(s.find("ConnectionFailed"), std::string::npos);
        ASSERT_NE(s.find("conn fail msg"), std::string::npos);
    }
    {
        auto err = core::connection_error::MissingConfig("cfg msg");
        auto s = err.to_str();
        ASSERT_NE(s.find("MissingConfig"), std::string::npos);
        ASSERT_NE(s.find("cfg msg"), std::string::npos);
    }
    {
        auto err = core::connection_error::FactoryNotRegistered("factory msg");
        auto s = err.to_str();
        ASSERT_NE(s.find("FactoryNotRegistered"), std::string::npos);
        ASSERT_NE(s.find("factory msg"), std::string::npos);
    }
    {
        auto err = core::connection_error::Timeout("timeout msg");
        auto s = err.to_str();
        ASSERT_NE(s.find("Timeout"), std::string::npos);
        ASSERT_NE(s.find("timeout msg"), std::string::npos);
    }
    {
        auto err = core::connection_error::SocketFailed("socket msg");
        auto s = err.to_str();
        ASSERT_NE(s.find("SocketFailed"), std::string::npos);
        ASSERT_NE(s.find("socket msg"), std::string::npos);
    }
    {
        auto err = core::connection_error::AuthFailed("auth msg");
        auto s = err.to_str();
        ASSERT_NE(s.find("AuthFailed"), std::string::npos);
        ASSERT_NE(s.find("auth msg"), std::string::npos);
    }
}

TEST(ConnectionErrorTest, ToStr_EmptyMessage_NoFormatPanic) {
    auto err = core::connection_error::Timeout("");
    const auto s = err.to_str();
    ASSERT_FALSE(s.empty());
    ASSERT_NE(s.find("Timeout"), std::string::npos);
}

TEST(ConnectionErrorTest, GetCode_IsCallableOnConst) {
    const auto err = core::connection_error::AuthFailed("x");
    ASSERT_EQ(err.get_code(), core::connection_error::type::AuthFailed);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
