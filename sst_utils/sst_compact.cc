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
    if (streq(in_fn, out_fn)) {
        printf("IN_FILE and OUT_FILE must be different!\n");
        exit(1);
    }
    Reader r(in_fn);
    write_sst(r, out_fn);
    return 0;
}
