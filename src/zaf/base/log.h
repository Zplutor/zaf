#pragma once

#include <sstream>

namespace zaf {
namespace internal {

class LogEntry {
public:
	~LogEntry();

	template<typename Type>
	LogEntry& operator<<(const Type& value) {
		stream_ << value;
		return *this;
	}

private:
	std::wostringstream stream_;
};

class LogEntryVodify {
public:
	void operator&(const LogEntry&) { }
};

}
}

#ifdef NDEBUG
#define IS_ZAFLOG_ENABLED false
#else
#define IS_ZAFLOG_ENABLED true
#endif

/**
 Write debug log to the output window.
 */
#define ZAFLOG() ! IS_ZAFLOG_ENABLED ? (void)0 : zaf::internal::LogEntryVodify() & zaf::internal::LogEntry()