#pragma once

#include <utility>
#include <string>

#include <stdio.h>
#include <errno.h>

#include "enumerator.h"

namespace sst {

class Writer {
public:
    Writer(const char* fpath);
    ~Writer() {
        if (fp_) {
            fclose(fp_);
        }
    }
    int get_error() const {
        return err_;
    }
    int write_pair(const std::pair<std::string, std::string>& pair, i32 flag = 0);
private:
    FILE* fp_;
    int err_;

    // for checking ordering
    bool first_;
    std::pair<std::string, std::string> last_;
};

template<class Iterator>
int write_sst(Iterator first, Iterator last, const char* fpath) {
    Writer* w = new Writer(fpath);
    int err = w->get_error();
    if (err != 0) {
        goto out;
    }
    while (first != last) {
        err = w->write_pair(*first);
        if (err != 0) {
            goto out;
        }
        ++first;
    }
out:
    delete w;
    return err;
}

int write_sst(Enumerator<std::pair<std::string, std::string>>* kv, const char* fpath);

} // namespace sst
