#pragma once

#include <cstddef>
#include <random>

extern std::mt19937 g_random_generator;

template<typename IntegerType>
IntegerType GenerateRandomInteger(IntegerType min, IntegerType max) {
    std::uniform_int_distribution<IntegerType> distribution(min, max);
    return distribution(g_random_generator);
}