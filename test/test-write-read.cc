#include <map>
#include <string>

#include "base/all.h"
#include "sstable/flags.h"
#include "sstable/reader.h"
#include "sstable/writer.h"

using namespace std;
using namespace sst;

TEST(sst, write) {
    map<string, string> m;
    m["hello"] = "world";
    m["abc"] = "123";
    write_sst(m.begin(), m.end(), "test.sst");
}

TEST(sst, write_then_read) {
    map<string, string> m;
    m["hello"] = "world";
    m["abc"] = "123";
    write_sst(m.begin(), m.end(), "test.sst");

    Reader* r = new Reader("test.sst");
    while (*r) {
        pair<string, string> p = r->next();
        Log::debug("%s -> %s", p.first.c_str(), p.second.c_str());
    }
    EXPECT_EQ(r->get_error(), 0);
    delete r;
}

TEST(sst, manual_write_then_read) {
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
    Writer* w = new Writer("test.sst");
    int record_cnt = 0;
    for (auto& it : m) {
        i32 flag = 0;
        if (it.first == "del_me") {
            flag |= Flags::DELETED;
        } else {
            record_cnt++;
        }
        w->write_pair(it, flag);
    }
    EXPECT_EQ(w->get_error(), 0);
    delete w;

    int read_cnt = 0;
    Reader* r = new Reader("test.sst");
    while (*r) {
        pair<string, string> p = r->next();
        Log::debug("%s -> %s", p.first.c_str(), p.second.c_str());
        read_cnt++;
    }
    EXPECT_EQ(r->get_error(), 0);
    EXPECT_EQ(record_cnt, read_cnt);
    delete r;
}
