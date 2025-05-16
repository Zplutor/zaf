#pragma once

#include <string_view>

namespace zaf {

/**
Represents a location in source code that is expressed by a file path, line number and function 
name.

@details
    Users should use the ZAF_SOURCE_LOCATION() macro to create instances of SourceLocation.
*/
class SourceLocation {
public:
    /**
    Constructs an empty instance without any location information.
    */
    SourceLocation() noexcept = default;

    /**
    Constructs an instance with the specified location information.

    @param file
        The file path of the source code. If it is null, an empty string will be used.

    @param line
        The line number of the source code.

    @param function
        The function name of the source code. If it is null, an empty string will be used.

    @note
        SourceLocation won't copy the string arguments. Users must ensure the strings are valid 
        during the lifecycle of the instance.
    */
    SourceLocation(const char* file, int line, const char* function) noexcept :
        file_(file ? file : ""), 
        line_(line),
        function_(function ? function : "") {

    }

    /**
    Gets the file path of the source code.

    @return
        The file path.
    */
    std::string_view File() const noexcept {
        return file_;
    }

    /**
    Gets the file name of the source code.

    @return
        The file name.
    */
    std::string_view FileName() const noexcept {

        auto file_path = this->File();
        auto last_delimiter_index = file_path.find_last_of("\\");
        if (last_delimiter_index == std::string_view::npos) {
            return file_path;
        }
        return file_path.substr(last_delimiter_index + 1);
    }

    /**
    Gets the line number of the source code.

    @return
        The line number.
    */
    int Line() const noexcept {
        return line_;
    }

    /**
    Gets the function name of the source code.

    @return
        The function name.
    */
    std::string_view Function() const noexcept {
        return function_;
    }

private:
    const char* file_{ "" };
    int line_{};
    const char* function_{ "" };
};


/**
Creates a SourceLocation instance with the location information where this macro is invoked.
*/
#define ZAF_SOURCE_LOCATION() zaf::SourceLocation{ __FILE__, __LINE__, __func__ }

}