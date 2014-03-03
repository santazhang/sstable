#include "flags.h"
#include "reader.h"

namespace sst {

Reader::Reader(const char* fpath): err_(0), cached_(false) {
    errno = 0;
    fp_ = fopen(fpath, "rb");
    if (fp_ == nullptr) {
        Log::error("cannot open file: %s", fpath);
        err_ = errno;
        return;
    }

    i32 magic = 0;
    if (fread(&magic, sizeof(magic), 1, fp_) != 1 || magic != Flags::magic) {
        Log::error("expect magic identifier to be 0x%08x, but got 0x%08x", Flags::magic, magic);
        err_ = EINVAL;
        fclose(fp_);
        fp_ = nullptr;
    }
}

bool Reader::prefetch_next() const {
    i32 flag = 0;
    for (;;) {
        errno = 0;
        if (fread(&flag, sizeof(flag), 1, fp_) != 1) {
            goto err_out;
        }

        // read key size varint
        char sbuf[9];
        errno = 0;
        sbuf[0] = fgetc(fp_);
        if (errno != 0) {
            goto err_out;
        }
        int bsize = SparseInt::buf_size(sbuf[0]);
        if (bsize > 1) {
            if (fread(sbuf + 1, 1, bsize - 1, fp_) != bsize - 1) {
                goto err_out;
            }
        }
        i32 key_len = SparseInt::load_i32(sbuf);

        if (Flags::deleted(flag)) {
            // ignore key
            fseek(fp_, key_len, SEEK_CUR);
        } else {
            next_.first.resize(key_len);
            fread(&next_.first[0], 1, key_len, fp_);
        }

        if (Flags::empty_value(flag)) {
            next_.second.resize(0);
        } else {
            errno = 0;
            sbuf[0] = fgetc(fp_);
            if (errno != 0) {
                goto err_out;
            }
            bsize = SparseInt::buf_size(sbuf[0]);
            if (bsize > 1) {
                if (fread(sbuf + 1, 1, bsize - 1, fp_) != bsize - 1) {
                    goto err_out;
                }
            }
            i32 val_len = SparseInt::load_i32(sbuf);

            if (Flags::deleted(flag)) {
                // ignore value
                fseek(fp_, val_len, SEEK_CUR);
            } else {
                next_.second.resize(val_len);
                fread(&next_.second[0], 1, val_len, fp_);
            }
        }

        if (!Flags::deleted(flag)) {
            break;
        }
    }
    cached_ = true;
    return true;

err_out:
    if (!feof(fp_)) {
        err_ = errno;
    }
    return false;
}

} // namespace sst
