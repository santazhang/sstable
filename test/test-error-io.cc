#include <map>
#include <string>

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "base/all.h"
#include "sstable/flags.h"
#include "sstable/reader.h"
#include "sstable/writer.h"

using namespace std;
using namespace sst;

TEST(error_io, print_errnos) {
    int errnos[] = {1, 2, 13};
    for (int i = 0; i < arraysize(errnos); i++) {
        Log::info("errno(%d): %s", errnos[i], strerror(errnos[i]));
    }
}

TEST(error_io, no_such_file) {
    Reader r("no_such_file.sst");
    EXPECT_FALSE(r.has_next());
    EXPECT_EQ(r.get_error(), ENOENT);
    EXPECT_FALSE(r.has_next());
}

TEST(error_io, read_not_allowed) {
    open("cannot_read.sst", O_CREAT | O_WRONLY | O_TRUNC, 0000);
    Reader r("cannot_read.sst");
    EXPECT_EQ(r.get_error(), EACCES);
    EXPECT_FALSE(r.has_next());
    EXPECT_FALSE(r.has_next());
    unlink("cannot_read.sst");
}

TEST(error_io, read_no_such_file) {
    Reader r("no_such_file.sst");
    EXPECT_EQ(r.get_error(), ENOENT);
    EXPECT_FALSE(r.has_next());
}

TEST(error_io, write_not_allowed) {
    open("cannot_write.sst", O_CREAT | O_WRONLY | O_TRUNC, 0000);
    Writer w("cannot_write.sst");
    EXPECT_EQ(w.get_error(), EACCES);
    unlink("cannot_write.sst");
}

TEST(error_io, write_not_possible) {
    Writer w("cannot_write/me.sst");
    EXPECT_EQ(w.get_error(), ENOENT);
}

TEST(error_io, magic_not_match) {
    const char* wrong_magic = "BLAH";
    for (int fsize = 4; fsize >= 0; fsize--) {
        FILE* fp = fopen("magic_not_match.sst", "wb");
        verify(fwrite(wrong_magic, 1, fsize, fp) == fsize);
        verify(fp != nullptr);
        fclose(fp);

        Reader r("magic_not_match.sst");
        EXPECT_FALSE(r.has_next());
        EXPECT_EQ(r.get_error(), EINVAL);
    }
    unlink("magic_not_match.sst");
}

static void write_dummy_data(const char* fn) {
    map<string, string> m;
    m["hello"] = "world";
    m["abc"] = "123";
    m["del_me"] = "you will not read me";
    m["empty_value"] = "";
    string creepy_value;
    creepy_value += '\0';
    creepy_value += "\1";
    creepy_value += "\7";
    creepy_value += "\017";
    m["very \"creepy\' \\ key \t  \v \f \? \n  \r \b \a 123"] = creepy_value;
    Writer* w = new Writer(fn);
    for (auto& it : m) {
        i32 flag = 0;
        if (it.first == "del_me") {
            flag |= Flags::DELETED;
        }
        w->write_pair(it, flag);
    }
    verify(w->get_error() == 0);
    delete w;
}

TEST(error_io, read_from_cropped_file) {
    write_dummy_data("cropped_file.sst");
    struct stat st;
    verify(stat("cropped_file.sst", &st) == 0);
    Log::debug("original file size: %d", st.st_size);
    for (int fsz = st.st_size; fsz >= 0; fsz--) {
        Log::debug("cropped file size to %d", fsz);
        truncate("cropped_file.sst", fsz);
        Reader r("cropped_file.sst");
        while (r.has_next()) {
            pair<string, string> p = r.next();
            Log::debug("%s => %s", p.first.c_str(), p.second.c_str());
        }
        if (r.get_error() != 0) {
            Log::debug("error = %d: %s", r.get_error(), strerror(r.get_error()));
        }
        if (fsz == st.st_size - 1) {
            EXPECT_NEQ(r.get_error(), 0);
        }
    }
    unlink("cropped_file.sst");
}
