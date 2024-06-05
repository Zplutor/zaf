#pragma once

#include <zaf/base/code_page.h>

namespace zaf {

struct XMLOutputOptions {
public:
    CodePage code_page{ CodePage::UTF8 };
    bool use_bom{ false };
};

}