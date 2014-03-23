#include <errno.h>

#include <map>
#include <string>
#include <sstream>
#include <iostream>

#include "sstable/writer.h"

using namespace sst;
using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("usage: %s OUT_FILE\n", argv[0]);
        exit(1);
    }
    const char* out_fn = argv[1];
    FILE* fp = fopen(out_fn, "wb");
    if (fp == nullptr) {
        printf("*** cannot open %s for writing: %s\n", out_fn, strerror(errno));
        exit(1);
    } else {
        fclose(fp);
    }

    printf("*** write key and values, separate them by space, tab or newline\n");
    printf("*** enter an empty line to stop\n");
    multimap<string, string> sst;

    string line;
    string key;
    string value;
    for (;;) {
        getline(cin, line);
        if (line == "") {
            break;
        }
        istringstream istr(line);
        while (istr) {
            if (key == "") {
                istr >> key;
            }
            if (value == "") {
                istr >> value;
            }
            if (key != "" && value != "") {
                printf("*** %s => %s\n", key.c_str(), value.c_str());
                insert_into_map(sst, key, value);
                key = "";
                value = "";
            }
        }
    }
    printf("writing %s...\n", out_fn);
    write_sst(sst.begin(), sst.end(), out_fn);
    return 0;
}
