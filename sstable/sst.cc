#include <stdio.h>

#include "sst.h"

using namespace std;
using namespace sstable;

namespace sstable {

int SST::write(const std::map<std::string, std::string>& src, const char* fpath) {
    int ret = 0;
    FILE* fp = fopen(fpath, "wb");

    if (fp == nullptr) {
        return -1;
    }

    for (auto& it : src) {
        char sbuf[9];
        int bsize = SparseInt::dump((i32) it.first.size(), sbuf);
        if (fwrite(sbuf, 1, bsize, fp) != bsize) {
            ret = -EIO;
            goto out;
        }
        if (fwrite(&it.first[0], 1, it.first.size(), fp) != it.first.size()) {
            ret = -EIO;
            goto out;
        }
        bsize = SparseInt::dump((i32) it.second.size(), sbuf);
        if (fwrite(sbuf, 1, bsize, fp) != bsize) {
            ret = -EIO;
            goto out;
        }
        if (fwrite(&it.second[0], 1, it.second.size(), fp) != it.second.size()) {
            ret = -EIO;
            goto out;
        }
    }

out:
    fclose(fp);
    return ret;
}

int SST::read(std::map<std::string, std::string>* dst, const char* fpath) {
    dst->clear();

    int ret = 0;
    FILE* fp = fopen(fpath, "rb");

    if (fp == nullptr) {
        return -1;
    }

    string key;
    string value;

    for (;;) {
        char sbuf[9];
        sbuf[0] = fgetc(fp);
        if (feof(fp)) {
            break;
        }
        int bsize = SparseInt::buf_size(sbuf[0]);
        if (bsize > 1) {
            if (fread(sbuf + 1, 1, bsize - 1, fp) != bsize - 1) {
                ret = -EIO;
                goto out;
            }
        }
        i32 key_len = SparseInt::load_i32(sbuf);
        key.resize(key_len);
        if (fread(&key[0], 1, key_len, fp) != key_len) {
            ret = -EIO;
            goto out;
        }
        sbuf[0] = fgetc(fp);
        if (feof(fp)) {
            ret = -EIO;
            goto out;
        }
        bsize = SparseInt::buf_size(sbuf[0]);
        if (bsize > 1) {
            if (fread(sbuf + 1, 1, bsize - 1, fp) != bsize - 1) {
                ret = -EIO;
                goto out;
            }
        }
        i32 value_len = SparseInt::load_i32(sbuf);
        value.resize(value_len);
        if (fread(&value[0], 1, value_len, fp) != value_len) {
            ret = -EIO;
            goto out;
        }
        insert_into_map(*dst, key, value);
    }

out:
    fclose(fp);
    return ret;
}

} // namespace sstable
