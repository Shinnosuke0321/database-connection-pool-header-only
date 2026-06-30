//
// Created by Shinnosuke Kawai on 6/15/26.
//
#include <gtest/gtest.h>
#include <core/result/unique_expected.h>

#include "core/memory/unique_function.h"

TEST(UniwueResultTest, DefultConstructorWithPrimitives) {
    core::result::unique_expected<int, float> result(23);

}

int main(int argc, char *argv[]) {

}
