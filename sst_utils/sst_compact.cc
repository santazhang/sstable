#include "sstable/reader.h"
#include "sstable/writer.h"

using namespace sst;

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("usage: %s IN_FILE OUT_FILE\n", argv[0]);
        exit(1);
    }
    char* in_fn = argv[1];
    char* out_fn = argv[2];
    Reader r(in_fn);
    write_sst(r, out_fn);
    return 0;
}
