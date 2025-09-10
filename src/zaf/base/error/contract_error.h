#pragma once

/**
@file
    Defines the `zaf::ContractError` class.
*/

#include <zaf/base/error/logic_error_base.h>

namespace zaf {

/**
The base class for errors related to contract programming violations.
*/
class ContractError : public LogicErrorBase {
public:
    using LogicErrorBase::LogicErrorBase;
};

}