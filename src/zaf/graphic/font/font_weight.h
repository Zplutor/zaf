#pragma once

#include <zaf/base/hash.h>
#include <zaf/base/relation_operator.h>
#include <zaf/graphic/font/font_weight_parser.h>
#include <zaf/object/equality.h>
#include <zaf/object/object.h>

namespace zaf {

/**
 Provides predefined values of font weight.
 */
class FontWeight : public Object {
public:
    ZAF_OBJECT;
    ZAF_DECLARE_EQUALITY

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
    FontWeight() = default;
    FontWeight(int value) : value_(value) { }

    int Value() const {
        return value_;
    }

    constexpr operator int() const {
        return value_;
    }

    std::wstring ToString() const override;

private:
    int value_{};
};

ZAF_OBJECT_BEGIN(FontWeight);
ZAF_OBJECT_PARSER(FontWeightParser);
ZAF_OBJECT_END;

}


namespace std {
template<>
struct hash<zaf::FontWeight> {
    std::size_t operator()(const zaf::FontWeight& font_weight) {
        return zaf::CalculateHash(font_weight.Value());
    }
};
}