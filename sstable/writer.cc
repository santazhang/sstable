#include <sys/types.h>
#include <sys/stat.h>

#include "utils.h"
#include "flags.h"
#include "writer.h"

namespace sst {

int write_sst(Enumerator<std::pair<std::string, std::string>>& kv, const char* fpath) {
   Writer* w = new Writer(fpath);
   int err = w->get_error();
   if (err != 0) {
       goto out;
   }
   while (kv) {
       err = w->write_pair(kv.next());
       if (err != 0) {
           goto out;
       }
   }
out:
   delete w;
   return err;
}

Writer::Writer(const char* fpath): err_(0), first_(false) {
    errno = 0;
    fp_ = fopen(fpath, "wb");
    if (fp_ == nullptr) {
        err_ = errno;
    } else {
        // magic numbers
        errno = 0;
        i32 magic = Flags::magic;
        if (fwrite(&magic, sizeof(magic), 1, fp_) != 1) {
            err_ = errno;
            if (err_ != 0) {
                err_ = EIO;
            }
        }
    }
}

Writer::~Writer() {
   if (fp_) {
       // make file read only
       fchmod(fileno(fp_), 0444);
       fclose(fp_);
   }
}

int Writer::write_pair(const std::pair<std::string, std::string>& pair, i32 flag /* =? */) {
    if (fp_ == nullptr) {
        return EBADF;
    }

    // check ordering
    if (first_) {
        first_ = false;
    } else {
        verify(last_ <= pair);
    }
    last_ = pair;

    const std::string& key = pair.first;
    const std::string& value = pair.second;

    if (flag == 0) {
        if (value.size() == 0) {
            assert(!Flags::empty_value(flag));
            flag |= Flags::EMPTY_VALUE;
            assert(Flags::empty_value(flag));
        }
    }

    char sbuf[9];
    int bsize = SparseInt::dump((i32) key.size(), sbuf);

    errno = 0;
    if (fwrite(&flag, sizeof(flag), 1, fp_) != 1) {
        goto err_out;
    }

    errno = 0;
    if ((int) fwrite(sbuf, 1, bsize, fp_) != bsize) {
        goto err_out;
    }
    errno = 0;
    if (fwrite(&key[0], 1, key.size(), fp_) != key.size()) {
        goto err_out;
    }
    if (!Flags::empty_value(flag)) {
        bsize = SparseInt::dump((i32) value.size(), sbuf);
        errno = 0;
        if ((int) fwrite(sbuf, 1, bsize, fp_) != bsize) {
            goto err_out;
        }
        errno = 0;
        if (fwrite(&value[0], 1, value.size(), fp_) != value.size()) {
            goto err_out;
        }
    }
    return 0;

err_out:
    err_ = errno;
    if (err_ == 0) {
        err_ = EIO;
    }
    return err_;
}

} // namespace sst
