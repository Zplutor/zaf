#include <zaf/base/log.h>
#include <Windows.h>
#include <ctime>
#include <iomanip>

namespace zaf {
namespace internal {

LogEntry::~LogEntry() {

    std::time_t time = std::time(nullptr);
    std::tm tm = { 0 };
    localtime_s(&tm, &time);

    std::wostringstream log_stream;
    log_stream << std::put_time(&tm, L"%H:%M:%S") << ' ' << stream_.str() << std::endl;

    std::wstring log_string = log_stream.str();
    OutputDebugString(log_string.c_str());
}

}
}