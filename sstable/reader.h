#pragma once

#include <utility>
#include <string>

#include <stdio.h>
#include <errno.h>

#include "utils.h"
#include "enumerator.h"

namespace sst {

class Reader: public Enumerator<std::pair<std::string, std::string>> {
public:
    Reader(const char* fpath);
    bool has_next() const {
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
protected:
    // protected dtor for RefCounted
    ~Reader() {
        if (fp_) {
            fclose(fp_);
            fp_ = nullptr;
        }
    }
private:
    bool prefetch_next() const;

    FILE* fp_;
    mutable int err_;

    mutable bool cached_;
    mutable std::pair<std::string, std::string> next_;
};

} // namespace sst
