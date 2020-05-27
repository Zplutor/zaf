#pragma once

namespace zaf {

class SourceSite {
public:
    SourceSite() = default;

    SourceSite(const char* file, int line, const char* function) : 
        file_(file ), 
        line_(line),
        function_(function) {

    }

    const char* GetFile() const {
        return file_;
    }

    int GetLine() const {
        return line_;
    }

    const char* GetFunction() const {
        return function_;
    }

private:
    const char* file_{};
    int line_{};
    const char* function_{};
};

#define ZAF_SOURCE_SITE() zaf::SourceSite{ __FILE__, __LINE__, __func__ }

}