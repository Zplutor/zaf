#pragma once

#include <ObjIdl.h>

namespace zaf::clipboard::internal {

STGMEDIUM ShallowCopySTGMEDIUM(const STGMEDIUM& medium);

STGMEDIUM WrapSTGMEDIUM(const STGMEDIUM& medium);

}