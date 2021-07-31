#pragma once

#include <zaf/object/object.h>

namespace zaf {

/**
 Provides predefined values of font weight.
 */
class FontWeight : public Object {
public:
    ZAF_DECLARE_TYPE

public:
    /**
     The minimum weight.
     */
    static const int Minimum = 1;

    /**
     Thin weight.
     */
    static const int Thin = 100;

    /**
     Extra light weight.
     */
    static const int ExtraLight = 200;

    /**
     Light weight.
     */
    static const int Light = 300;

    /**
     Semi light weight.
     */
    static const int SemiLight = 350;

    /**
     Regular weight.
     */
    static const int Regular = 400;

    /**
     Medium weight.
     */
    static const int Medium = 500;

    /**
     Semi bold weight.
     */
    static const int SemiBold = 600;

    /**
     Bold weight.
     */
    static const int Bold = 700;

    /**
     Extra bold weight.
     */
    static const int ExtraBold = 800;

    /**
     Black weight.
     */
    static const int Black = 900;

    /**
     Extra black weight.
     */
    static const int ExtraBlack = 950;

    /**
     The maximum weight.
     */
    static const int Maximum = 999;

public:
    constexpr FontWeight() = default;
    constexpr FontWeight(int value) : value_(value) { }

    constexpr operator int() const {
        return value_;
    }

private:
    int value_{};
};

}