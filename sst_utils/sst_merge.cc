#include <vector>
#include <queue>

#include "sstable/reader.h"
#include "sstable/writer.h"

using namespace sst;
using namespace std;

struct merge_helper {
    pair<string, string> data;
    Reader* src;

    merge_helper() {}
    merge_helper(const pair<string, string>& data, Reader* src): data(data), src(src) {}
};

inline bool operator < (const struct merge_helper& a, const struct merge_helper& b) {
    // hack: reverse order, so priority_queue is a min-heap
    return a.data > b.data;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("usage: %s IN_FILE.. OUT_FILE\n", argv[0]);
        exit(1);
    }
    vector<Reader*> readers;
    for (int i = 1; i < argc - 1; i++) {
        readers.push_back(new Reader(argv[i]));
    }
    Writer w(argv[argc - 1]);

    priority_queue<merge_helper> q;
    // bootstrap
    for (auto& it : readers) {
        if (it->has_next()) {
            q.push(merge_helper(it->next(), it));
        }
    }
    // merging
    while (!q.empty()) {
        merge_helper mh = q.top();
        w.write_pair(mh.data);
        if (mh.src->has_next()) {
            q.push(merge_helper(mh.src->next(), mh.src));
        }
        q.pop();
    }

    for (auto& it : readers) {
        delete it;
    }
    return 0;
}
