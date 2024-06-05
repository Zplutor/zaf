#pragma once

#include <zaf/base/code_page.h>

namespace zaf {

struct XMLInputOptions {
    CodePage code_page{ CodePage::UTF8 };
};

}