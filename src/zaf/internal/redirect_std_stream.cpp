#include "redirect_std_stream.h"
#include <Windows.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>

namespace zaf {
namespace internal {

static FILE* MapHandleToFile(HANDLE handle);

void RedirectStandardStream() {

    if (! AllocConsole()) {
        return;
    }

    HANDLE stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    FILE* stdoutFile = MapHandleToFile(stdoutHandle);
    *stdout = *stdoutFile;

    HANDLE stderrHandle = GetStdHandle(STD_ERROR_HANDLE);
    FILE* stderrFile = MapHandleToFile(stderrHandle);
    *stderr = *stderrFile;
}


static FILE* MapHandleToFile(HANDLE handle) {

    int cHandle = _open_osfhandle(intptr_t(handle), _O_TEXT);
    FILE* file = _fdopen(cHandle, "w");
    return file;
}

}
}