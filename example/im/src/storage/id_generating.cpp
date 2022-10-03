#include "storage/id_generating.h"

Id GenerateId() {
    static Id seed = 0;
    return ++seed;
}