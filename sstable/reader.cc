#include "flags.h"
#include "reader.h"

namespace sst {

Reader::Reader(const char* fpath): err_(0), cached_(false) {
    errno = 0;
    fp_ = fopen(fpath, "rb");
    if (fp_ == nullptr) {
        err_ = errno;
        Log::error("cannot open file: %s, error=%d: %s", fpath, err_, strerror(err_));
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

bool Reader::prefetch_next() {
    verify(cached_ == false);

    if (fp_ == nullptr) {
        return false;
    }

    i32 flag = 0;
    for (;;) {
        errno = 0;
        long before_pos = ftell(fp_);
        if (fread(&flag, sizeof(flag), 1, fp_) != 1) {
            long after_pos = ftell(fp_);
            if (before_pos == after_pos) {
                return false;
            }
            goto err_out;
        }

        // read key size varint
        char sbuf[9];
        errno = 0;
        sbuf[0] = fgetc(fp_);
        if (errno != 0 || feof(fp_)) {
            goto err_out;
        }
        int bsize = SparseInt::buf_size(sbuf[0]);
        if (bsize > 1) {
            if ((int) fread(sbuf + 1, 1, bsize - 1, fp_) != bsize - 1) {
                goto err_out;
            }
        }
        i32 key_len = SparseInt::load_i32(sbuf);

        if (Flags::deleted(flag)) {
            // ignore key
            long old_pos = ftell(fp_);
            if (fseek(fp_, key_len, SEEK_CUR) != 0) {
                goto err_out;
            }
            long new_pos = ftell(fp_);
            if (new_pos - old_pos != key_len) {
                goto err_out;
            }
        } else {
            next_.first.resize(key_len);
            if ((int) fread(&next_.first[0], 1, key_len, fp_) != key_len) {
                goto err_out;
            }
        }

        if (Flags::empty_value(flag)) {
            next_.second.resize(0);
        } else {
            errno = 0;
            sbuf[0] = fgetc(fp_);
            if (errno != 0 || feof(fp_)) {
                goto err_out;
            }
            bsize = SparseInt::buf_size(sbuf[0]);
            if (bsize > 1) {
                if ((int) fread(sbuf + 1, 1, bsize - 1, fp_) != bsize - 1) {
                    goto err_out;
                }
            }
            i32 val_len = SparseInt::load_i32(sbuf);

            if (Flags::deleted(flag)) {
                // ignore value
                long old_pos = ftell(fp_);
                if (fseek(fp_, val_len, SEEK_CUR) != 0) {
                    goto err_out;
                }
                long new_pos = ftell(fp_);
                if (new_pos - old_pos != val_len) {
                    goto err_out;
                }
            } else {
                next_.second.resize(val_len);
                if ((int) fread(&next_.second[0], 1, val_len, fp_) != val_len) {
                    goto err_out;
                }
            }
        }

        if (!Flags::deleted(flag)) {
            break;
        }
    }
    cached_ = true;
    return true;

err_out:
    err_ = errno;
    if (err_ == 0) {
        err_ = EIO;
    }
    return false;
}

} // namespace sst
