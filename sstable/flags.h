#pragma once

#include "utils.h"

namespace sst {

class Flags {
public:
    static const i32 magic = 0x31545353; // 'SST1'

    static const i32 DELETED = 0x1;
    static const i32 EMPTY_VALUE = 0x2;
    
    static inline bool deleted(i32 flag) {
        return flag & DELETED;
    }
    
    static inline bool empty_value(i32 flag) {
        return flag & EMPTY_VALUE;
    }
};

} // namespace sst
