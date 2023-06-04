#include <zaf/clipboard/internal/medium_wrapping.h>
#include <zaf/base/error/check.h>
#include <zaf/clipboard/internal/medium_wrapper.h>

namespace zaf::clipboard::internal {
namespace {

bool DoesSTGMEDIUMNeedWrapping(const STGMEDIUM& medium) {

    if (medium.tymed == TYMED_GDI ||
        medium.tymed == TYMED_MFPICT ||
        medium.tymed == TYMED_ENHMF ||
        medium.tymed == TYMED_HGLOBAL ||
        medium.tymed == TYMED_FILE) {

        return !medium.pUnkForRelease;
    }

    return false;
}

}


STGMEDIUM ShallowCopySTGMEDIUM(const STGMEDIUM& medium) {

    auto result = medium;

    if (result.pUnkForRelease) {
        result.pUnkForRelease->AddRef();
    }

    switch (result.tymed) {
    case TYMED_FILE: {
        auto buffer_length = std::wcslen(result.lpszFileName) + 1;
        auto new_string = reinterpret_cast<wchar_t*>(
            CoTaskMemAlloc(buffer_length * sizeof(wchar_t)));
        wcscpy_s(new_string, buffer_length, result.lpszFileName);
        result.lpszFileName = new_string;
        break;
    }
    case TYMED_ISTREAM:
        result.pstm->AddRef();
        break;
    case TYMED_ISTORAGE:
        result.pstg->AddRef();
        break;
    default:
        break;
    }

    return result;
}


STGMEDIUM WrapSTGMEDIUM(const STGMEDIUM& medium) {

    if (!DoesSTGMEDIUMNeedWrapping(medium)) {
        return medium;
    }
    
    STGMEDIUM wrapper = ShallowCopySTGMEDIUM(medium);
    wrapper.pUnkForRelease = new MediumWrapper{ medium };
    return wrapper;
}

}