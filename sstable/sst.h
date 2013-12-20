#pragma once

#include <string>
#include <map>

#include "utils.h"

namespace sstable {

class SST {
public:
    static int write(const std::map<std::string, std::string>& src, const char* fpath);
    static int read(std::map<std::string, std::string>* dst, const char* fpath);
};

} // namespace sstable
