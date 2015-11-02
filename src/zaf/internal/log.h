#pragma once

#include <iostream>
#include <sstream>

namespace zaf {
namespace internal {

class LogEntry {
public:
	~LogEntry() {
		std::wcout << stream_.str() << std::endl;
	}

	std::wostringstream& GetStream() {
		return stream_;
	}

private:
	std::wostringstream stream_;
};

}
}

#define LOG() zaf::internal::LogEntry().GetStream()