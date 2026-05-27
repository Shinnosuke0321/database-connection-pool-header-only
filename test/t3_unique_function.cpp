#include <gtest/gtest.h>
#include <core/memory/unique_function.h>
#include <memory>

using namespace core::memory;

class large_class_t {
public:
    large_class_t() = default;
    large_class_t(const large_class_t&) = default;
    large_class_t(large_class_t&&) = default;
    large_class_t& operator=(const large_class_t& other) = default;
    large_class_t& operator=(large_class_t&&) = default;
    ~large_class_t() = default;

    std::string v1,v2,v3 = "asfvblio34r1380rv7goiufedvo3r78vq0hodineqh087";
};

// Core functionality
TEST(UniqueFunctionTest, StoreAndInvokeSimpleLambda) {
    auto fn = unique_function<int(int, int)>([](int a, int b) { return a + b; });
    EXPECT_EQ(fn(2, 3), 5);
}

TEST(UniqueFunctionTest, StoreAndInvokeVoidLambda) {
    int called = 0;
    auto fn = unique_function<void()>([&called]() { called++; });
    fn();
    EXPECT_EQ(called, 1);
}

TEST(UniqueFunctionTest, StoreMoveOnlyCallable) {
    auto ptr = std::make_unique<int>(42);

    unique_function<int()> fn = [p = std::move(ptr)]() { return *p; };
    EXPECT_EQ(fn(), 42);
}

TEST(UniqueFunctionTest, StoreMixedSemanticsCallable) {
    auto ptr = std::make_unique<int>(42);
    large_class_t large_obj;
    const unique_function<int()> fn = [large_obj, p = std::move(ptr)]() mutable {
        return *p;
    };
    EXPECT_EQ(fn(), 42);
}

TEST(UniqueFunctionTest, CaptureMoveOnlyFuncToAnotherMoveOnly) {
    unique_function<int()> fn_to_capture = []() { return 42; };
    large_class_t copiable_res;
    unique_function<int()> fn = [copiable_res, fn = std::move(fn_to_capture)]() mutable {
        return fn();
    };
    EXPECT_FALSE(fn_to_capture);
    EXPECT_EQ(fn(), 42);
}

static int add(int a, int b) { return a + b; }

TEST(UniqueFunctionTest, StoreFunctionPointer) {
    unique_function<int(int, int)> fn = &add;
    EXPECT_EQ(fn(2, 3), 5);
}

// Move semantics
TEST(UniqueFunctionTest, MoveConstructorTransfersOwnership) {
    unique_function<int()> fn1 = []() { return 42; };
    unique_function<int()> fn2 = std::move(fn1);
    EXPECT_FALSE(fn1);
    EXPECT_TRUE(fn2);
    EXPECT_EQ(fn2(), 42);
}

TEST(UniqueFunctionTest, MoveAssignmentTransfersOwnership) {
    unique_function<int()> fn1 = []() { return 1; };
    unique_function<int()> fn2 = []() { return 2; };
    fn1 = std::move(fn2);
    EXPECT_FALSE(fn2);
    EXPECT_EQ(fn1(), 2);
}

// Empty state
TEST(UniqueFunctionTest, DefaultConstructorCreatesEmpty) {
    unique_function<int()> fn;
    EXPECT_FALSE(fn);
}

TEST(UniqueFunctionTest, CallingEmptyThrowsException) {
    unique_function<int()> fn;
    EXPECT_THROW(fn(), std::bad_function_call);
}

TEST(UniqueFunctionTest, AssignNullptrClearsFunction) {
    unique_function<int()> fn = []() { return 42; };
    fn = nullptr;
    EXPECT_FALSE(fn);
    EXPECT_THROW(fn(), std::bad_function_call);
}

TEST(UniqueFunctionTest, ConstructFromNullptr) {
    unique_function<int()> fn = nullptr;
    EXPECT_FALSE(fn);
}

// Utility
TEST(UniqueFunctionTest, SwapExchangesCallables) {
    unique_function<int()> fn1 = []() { return 1; };
    unique_function<int()> fn2 = []() { return 2; };
    fn1.swap(fn2);
    EXPECT_EQ(fn1(), 2);
    EXPECT_EQ(fn2(), 1);
}

TEST(UniqueFunctionTest, ConfirmCallableToBeNull) {
    unique_function<int()> fn1 = nullptr;
    EXPECT_TRUE(fn1 == nullptr);
}

TEST(UniqueFunctionTest, ConfirmTwoCallableToBeNull) {
    unique_function<int()> fn1 = nullptr;
    unique_function<int()> fn2 = nullptr;
    EXPECT_TRUE(fn1 == fn2);
}

TEST(UniqueFunctionTest, CampareTwoFunctions) {
    unique_function<int()> fn1 = []() {return 1;};
    unique_function<int()> fn2 = []() {return 2;};
    EXPECT_FALSE(fn1 == fn2);
}

TEST(UniqueFunctionTest, ConstRefQualifiedCall) {
    unique_function<int(int)> fn = [] (int a) { return a; };
    EXPECT_EQ(fn(42), 42);
    const auto& fn2 = fn;
    EXPECT_EQ(fn2(42), 42);
}

TEST(UniqueFunctionTest, NonConstRefQualifiedCall) {
    unique_function<int(int)> fn = [] (int a) { return a; };
    EXPECT_EQ(fn(42), 42);
    auto& fn2 = fn;
    EXPECT_EQ(fn2(42), 42);
}

TEST(UniqueFunctionTest, RValueRefQualifiedCall) {
    unique_function<int(int)> fn = [] (int a) { return a; };
    EXPECT_EQ(std::move(fn)(42), 42);
}

int main(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
