#pragma once

#include <string>

class TestObject {
public:
    TestObject() {
        default_constructor_called = true;
    }

    TestObject(const TestObject& other) : value(other.value) {
        copy_constructor_called = true;
    }

    TestObject(TestObject&& other) : value(std::move(other.value)) {
        move_constructor_called = true;
    }

public:
    bool default_constructor_called = false;
    bool copy_constructor_called = false;
    bool move_constructor_called = false;
    std::string value;
};