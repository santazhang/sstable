#include <map>
#include <string>

#include "base/all.h"
#include "sstable/sst.h"

using namespace std;
using namespace sstable;

TEST(sst_writer, write) {
    map<string, string> m;
    m["hello"] = "world";
    SST::write(m, "test-writer.sst");
}


TEST(sst_writer, write_then_read) {
    map<string, string> m;
    m["hello"] = "world";
    SST::write(m, "test-writer.sst");

    map<string, string> m_read;
    SST::read(&m_read, "test-writer.sst");
    EXPECT_TRUE(m == m_read);
}
