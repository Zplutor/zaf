#pragma once

#include <Propvarutil.h>
#include <wincodec.h>
#include <cstdint>
#include <functional>
#include <zaf/base/com_object.h>
#include <zaf/base/error.h>

namespace zaf {

class ImageMetadataQuerier : public ComObject<IWICMetadataQueryReader> {
public:
    ImageMetadataQuerier() { }
    explicit ImageMetadataQuerier(IWICMetadataQueryReader* handle) : ComObject(handle) { }

    template<typename ValueType>
    ValueType QueryMetadata(const std::wstring& query_expression) const {
        std::error_code error_code;
        auto result = QueryMetadata<ValueType>(query_expression, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    template<typename ValueType>
    ValueType QueryMetadata(const std::wstring& query_expression, std::error_code& error_code) const {
        return GetMetadata(query_expression, error_code, Converter<ValueType>::Convert);
    }

private:
    template<typename ValueType>
    ValueType GetMetadata(
        const std::wstring& query_expression,
        std::error_code& error_code,
        const std::function<HRESULT(const PROPVARIANT&, ValueType&)>& get_value) const {

        ValueType value{ };

        PROPVARIANT variant = { 0 };
        HRESULT result = GetHandle()->GetMetadataByName(query_expression.c_str(), &variant);
        error_code = MakeComErrorCode(result);
        if (error_code) {
            return value;
        }

        result = get_value(variant, value);
        error_code = MakeComErrorCode(result);
        PropVariantClear(variant);
        return value;
    }

    template<typename ValueType>
    struct Converter { };

    template<>
    struct Converter<bool> {
        HRESULT Convert(const PROPVARIANT& variant, bool& value) {
            BOOL bool_value = FALSE;
            HRESULT result = PropVariantToBoolean(variant, &bool_value);
            value = !!bool_value;
            return result;
        }
    };

    template<>
    struct Converter<std::int16_t> {
        HRESULT Convert(const PROPVARIANT& variant, std::int16_t& value) {
            return PropVariantToInt16(variant, &value);
        }
    };

    template<>
    struct Converter<std::uint16_t> {
        HRESULT Convert(const PROPVARIANT& variant, std::uint16_t& value) {
            return PropVariantToUInt16(variant, &value);
        }
    };

    template<>
    struct Converter<std::int32_t> {
        HRESULT Convert(const PROPVARIANT& variant, std::int32_t& value) {
            return PropVariantToInt32(variant, &value);
        }
    };

    template<>
    struct Converter<std::uint32_t> {
        HRESULT Convert(const PROPVARIANT& variant, std::uint32_t& value) {
            return PropVariantToUInt32(variant, &value);
        }
    };

    template<>
    struct Converter<std::int64_t> {
        HRESULT Convert(const PROPVARIANT& variant, std::int64_t& value) {
            return PropVariantToInt64(variant, &value);
        }
    };

    template<>
    struct Converter<std::uint64_t> {
        HRESULT Convert(const PROPVARIANT& variant, std::uint64_t& value) {
            return PropVariantToUInt64(variant, &value);
        }
    };

    template<>
    struct Converter<double> {
        HRESULT Convert(const PROPVARIANT& variant, double& value) {
            return PropVariantToDouble(variant, &value);
        }
    };

    template<>
    struct Converter<std::wstring> {
        HRESULT Convert(const PROPVARIANT& variant, std::wstring& value) {
            wchar_t* string = nullptr;
            HRESULT result = PropVariantToStringAlloc(variant, &string);
            if (string != nullptr) {
                value = string;
                CoTaskMemFree(string);
            }
            return result;
        }
    };

    template<>
    struct Converter<ImageMetadataQuerier> {
        HRESULT Convert(const PROPVARIANT& variant, ImageMetadataQuerier& value) {

            if (variant.vt != VT_UNKNOWN) {
                value.Reset();
                return DISP_E_TYPEMISMATCH;
            }

            IWICMetadataQueryReader* handle = nullptr;
            HRESULT result = variant.punkVal->QueryInterface(&handle);

            value = ImageMetadataQuerier(handle);
            return result;
        }
    };
};

}