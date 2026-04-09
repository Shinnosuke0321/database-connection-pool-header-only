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
}