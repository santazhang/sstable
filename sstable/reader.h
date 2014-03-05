#pragma once

#include <utility>
#include <string>

#include <stdio.h>
#include <errno.h>

#include "utils.h"
#include "enumerator.h"

namespace sst {

class Reader: public NoCopy, public Enumerator<std::pair<std::string, std::string>> {
public:
    Reader(const char* fpath);
    ~Reader() {
        if (fp_) {
            fclose(fp_);
            fp_ = nullptr;
        }
    }
    bool has_next() {
        if (cached_) {
            return true;
        } else {
            return prefetch_next();
        }
    }
    int get_error() const {
        return err_;
    }
    std::pair<std::string, std::string> next() {
        if (!cached_) {
            verify(prefetch_next());
        }
        cached_ = false;
        return next_;
    }
private:
    bool prefetch_next();

    FILE* fp_;
    int err_;

    bool cached_;
    std::pair<std::string, std::string> next_;
};

} // namespace sst
