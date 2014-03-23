#include <vector>
#include <queue>

#include "sstable/reader.h"
#include "sstable/writer.h"

using namespace sst;
using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("usage: %s IN_FILE.. OUT_FILE\n", argv[0]);
        exit(1);
    }
    MergedEnumerator<std::pair<std::string, std::string>> merged_stream;
    vector<Reader*> readers;
    for (int i = 1; i < argc - 1; i++) {
        Reader* r = new Reader(argv[i]);
        readers.push_back(r);
        merged_stream.add_source(r);
    }
    char* out_fn = argv[argc - 1];
    write_sst(merged_stream, out_fn);

    for (auto& it : readers) {
        delete it;
    }
    return 0;
}
