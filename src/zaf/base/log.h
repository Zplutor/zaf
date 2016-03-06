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
#define ZAF_IS_LOG_ENABLED false
#else
#define ZAF_IS_LOG_ENABLED true
#endif

/**
 Write debug log to the output window.
 */
#define ZAF_LOG() ! ZAF_IS_LOG_ENABLED ? (void)0 : zaf::internal::LogEntryVodify() & zaf::internal::LogEntry()