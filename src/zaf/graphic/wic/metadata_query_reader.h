#pragma once

#include <Windows.h>
#include <Propvarutil.h>
#include <wincodec.h>
#include <cstdint>
#include <functional>
#include <vector>
#include <zaf/base/com_object.h>
#include <zaf/base/error/com_error.h>

#undef max
#undef min

namespace zaf::wic {

class MetadataQueryReader : public COMObject<IWICMetadataQueryReader> {
public:
    using COMObject::COMObject;

    template<typename ValueType>
    ValueType QueryMetadata(const std::wstring& query_expression) const {
        return GetMetadata(
            query_expression, 
            std::function<HRESULT(const PROPVARIANT&, ValueType&)>(Converter<ValueType>::Convert));
    }

private:
    template<typename ValueType>
    ValueType GetMetadata(
        const std::wstring& query_expression,
        const std::function<HRESULT(const PROPVARIANT&, ValueType&)>& get_value) const {

        ValueType value{ };

        PROPVARIANT variant = { 0 };
        HRESULT result = Ptr()->GetMetadataByName(query_expression.c_str(), &variant);
        ZAF_THROW_IF_COM_ERROR(result);

        result = get_value(variant, value);
        ZAF_THROW_IF_COM_ERROR(result);

        PropVariantClear(&variant);
        return value;
    }

    template<typename ValueType>
    struct Converter { };

    template<>
    struct Converter<bool> {
        static HRESULT Convert(const PROPVARIANT& variant, bool& value) {
            BOOL bool_value = FALSE;
            HRESULT result = PropVariantToBoolean(variant, &bool_value);
            value = !!bool_value;
            return result;
        }
    };

    template<>
    struct Converter<std::int8_t> {
        static HRESULT Convert(const PROPVARIANT& variant, std::int8_t& value) {
            std::int16_t int16_value = 0;
            HRESULT com_error = PropVariantToInt16(variant, &int16_value);
            if (FAILED(com_error)) {
                return com_error;
            }
            if ((int16_value < std::numeric_limits<std::int8_t>::min()) ||
                (std::numeric_limits<std::int8_t>::max() < int16_value)) {
                return E_BOUNDS;
            }
            value = static_cast<std::int8_t>(int16_value);
            return S_OK;
        }
    };

    template<>
    struct Converter<std::uint8_t> {
        static HRESULT Convert(const PROPVARIANT& variant, std::uint8_t& value) {
            std::uint16_t uint16_value = 0;
            HRESULT com_error = PropVariantToUInt16(variant, &uint16_value);
            if (FAILED(com_error)) {
                return com_error;
            }
            if (std::numeric_limits<std::uint8_t>::max() < uint16_value) {
                return E_BOUNDS;
            }
            value = static_cast<std::uint8_t>(uint16_value);
            return S_OK;
        }
    };

    template<>
    struct Converter<std::int16_t> {
        static HRESULT Convert(const PROPVARIANT& variant, std::int16_t& value) {
            return PropVariantToInt16(variant, &value);
        }
    };

    template<>
    struct Converter<std::uint16_t> {
        static HRESULT Convert(const PROPVARIANT& variant, std::uint16_t& value) {
            return PropVariantToUInt16(variant, &value);
        }
    };

    template<>
    struct Converter<std::int32_t> {
        static HRESULT Convert(const PROPVARIANT& variant, std::int32_t& value) {
            return PropVariantToInt32(variant, &value);
        }
    };

    template<>
    struct Converter<std::uint32_t> {
        static HRESULT Convert(const PROPVARIANT& variant, std::uint32_t& value) {
            return PropVariantToUInt32(variant, &value);
        }
    };

    template<>
    struct Converter<std::int64_t> {
        static HRESULT Convert(const PROPVARIANT& variant, std::int64_t& value) {
            return PropVariantToInt64(variant, &value);
        }
    };

    template<>
    struct Converter<std::uint64_t> {
        static HRESULT Convert(const PROPVARIANT& variant, std::uint64_t& value) {
            return PropVariantToUInt64(variant, &value);
        }
    };

    template<>
    struct Converter<double> {
        static HRESULT Convert(const PROPVARIANT& variant, double& value) {
            return PropVariantToDouble(variant, &value);
        }
    };

    template<>
    struct Converter<std::vector<std::uint8_t>> {
        static HRESULT Convert(const PROPVARIANT& variant, std::vector<std::uint8_t>& value) {
            if (variant.vt == (VT_UI1 | VT_VECTOR)) {
                value.resize(variant.caub.cElems);
                std::memcpy(&value[0], variant.caub.pElems, variant.caub.cElems);
                return S_OK;
            }
            else {
                return E_FAIL;
            }
        }
    };

    template<>
    struct Converter<std::wstring> {
        static HRESULT Convert(const PROPVARIANT& variant, std::wstring& value) {
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
    struct Converter<MetadataQueryReader> {
        static HRESULT Convert(const PROPVARIANT& variant, MetadataQueryReader& value) {

            if (variant.vt != VT_UNKNOWN) {
                value = MetadataQueryReader{};
                return DISP_E_TYPEMISMATCH;
            }

            COMPtr<IWICMetadataQueryReader> inner;
            HRESULT result = variant.punkVal->QueryInterface(inner.Reset());

            value = MetadataQueryReader{ inner };
            return result;
        }
    };
};

}