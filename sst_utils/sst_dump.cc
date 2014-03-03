#include <ctype.h>

#include "sstable/reader.h"
#include "sstable/writer.h"

using namespace sst;
using namespace std;

void print_str(const string& str) {
    for (size_t i = 0; i < str.size(); i++) {
        char ch = str[i];
        if (ch == '"') {
            fputc('\\', stdout);
            fputc('"', stdout);
        } else if (ch == '\t') {
            fputc('\\', stdout);
            fputc('t', stdout);
        } else if (ch == '\'') {
            fputc('\\', stdout);
            fputc('\'', stdout);
        } else if (ch == '\n') {
            fputc('\\', stdout);
            fputc('n', stdout);
        } else if (ch == '\r') {
            fputc('\\', stdout);
            fputc('r', stdout);
        } else if (ch == '\b') {
            fputc('\\', stdout);
            fputc('b', stdout);
        } else if (ch == '\a') {
            fputc('\\', stdout);
            fputc('a', stdout);
        } else if (ch == '\\') {
            fputc('\\', stdout);
            fputc('\\', stdout);
        } else if (ch == '\?') {
            fputc('\\', stdout);
            fputc('?', stdout);
        } else if (ch == '\v') {
            fputc('\\', stdout);
            fputc('v', stdout);
        } else if (ch == '\f') {
            fputc('\\', stdout);
            fputc('f', stdout);
        } else if (isprint(ch)) {
            fputc(ch, stdout);
        } else {
            unsigned u = (unsigned) ch;
            if (u < 8) {
                printf("\\%o", u);
            } else {
                printf("\\0%o", u);
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("usage: %s FILE\n", argv[0]);
        exit(1);
    }
    char* fn = argv[1];
    Reader r(fn);
    while (r) {
        pair<string, string> p = r.next();
        fputc('"', stdout);
        print_str(p.first);
        fputc('"', stdout);
        printf("  =>  ");
        fputc('"', stdout);
        print_str(p.second);
        fputc('"', stdout);
        fputc('\n', stdout);
    }
    return 0;
}
