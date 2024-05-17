#pragma once

/**
@file
    Defines the class zaf::clipboard::DataDescriptor.
*/

#include <objidl.h>
#include <zaf/clipboard/data_aspect.h>
#include <zaf/clipboard/format_type.h>
#include <zaf/clipboard/medium_type.h>

namespace zaf::clipboard {

/**
An encapsulation class for the FORMATETC structure that describes various attributes of a clipboard
data, such as the format type and the supported medium types.

@details
    The name of the FORMATETC structure is not descriptive enough and may cause confusion, so it is
    encapsulated and renamed to a more accurate class, DataDescriptor.
*/
class DataDescriptor {
public:
    /**
    Creates a DataDescriptor with the specified format type.

    @param format_type
        The format type to be set in the DataDescriptor.

    @return
        The DataDescriptor with the specified format type, with its medium types set to 
        MediumType::GlobalMem.
    */
    static DataDescriptor FromFormatType(clipboard::FormatType format_type) noexcept;

    /**
    Creates a DataDescriptor with the specified format type and the specified medium types.

    @param format_type
        The format type to be set in the DataDescriptor.

    @param medium_types
        The medium types to be set in the DataDescriptor. It may contains one or more types 
        combined using bitwise operations.

    @return
        The DataDescriptor with the specified format type and medium types.
    */
    static DataDescriptor FromFormatType(
        clipboard::FormatType format_type,
        clipboard::MediumType medium_types) noexcept;

public:
    /**
    Constructs an instance with default values.
    */
    DataDescriptor() noexcept;

    /**
    Constructs an instance from the specified FORMATETC structure.
    */
    explicit DataDescriptor(const FORMATETC& inner) noexcept;

    clipboard::FormatType FormatType() const noexcept {
        return static_cast<clipboard::FormatType>(inner_.cfFormat);
    }

    void SetFormatType(clipboard::FormatType format_type) noexcept {
        inner_.cfFormat = static_cast<CLIPFORMAT>(format_type);
    }

    MediumType MediumTypes() const noexcept {
        return static_cast<clipboard::MediumType>(inner_.tymed);
    }

    void SetMediumTypes(clipboard::MediumType medium_type) noexcept {
        inner_.tymed = static_cast<DWORD>(medium_type);
    }

    DataAspect Aspect() const noexcept {
        return static_cast<DataAspect>(inner_.dwAspect);
    }

    void SetAspect(DataAspect aspect) {
        inner_.dwAspect = static_cast<DWORD>(aspect);
    }

    int AspectIndex() const noexcept {
        return static_cast<int>(inner_.lindex);
    }

    void SetAspectIndex(int index) {
        inner_.lindex = static_cast<LONG>(index);
    }

    const FORMATETC& Inner() const noexcept {
        return inner_;
    }

    bool operator==(const DataDescriptor& other) const;
    bool operator==(const FORMATETC& other) const;

private:
    FORMATETC inner_{};
};

}