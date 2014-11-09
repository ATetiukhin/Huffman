#include <iostream>
#include <cstring>
#include "huffman.hpp"

/* Debug memory check support */
/* Debug memory allocation support */
#ifdef _DEBUG
#  define _CRTDBG_MAP_ALLOC
#  include <crtdbg.h>
#  define SetDbgMemHooks() \
  _CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF | \
  _CRTDBG_ALLOC_MEM_DF | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG))
#endif /* _DEBUG */

#ifdef _DEBUG
#  ifdef _CRTDBG_MAP_ALLOC
#    define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#  endif /* _CRTDBG_MAP_ALLOC */
#endif /* _DEBUG */

class InvalidateArguments : public std::exception {
    virtual const char *what() const throw() {
        return "invalidate arguments";
    }
};

int main(int argc, char *argv[]) {
    /* Debug memory check support */
#ifdef _DEBUG
        SetDbgMemHooks();
    #endif

    std::ios_base::sync_with_stdio(0);
    Huffman compression;

    try {
        if (argc != 6) {
            throw InvalidateArguments();
        } else if (strcmp(argv[2], "-f") || strcmp(argv[4], "-o")) {
            throw InvalidateArguments();
        }

        if (!strcmp(argv[1], "-c")) {
            compression.encode_file(argv[3], argv[5]);
        } else if (!strcmp(argv[1], "-u")) {
            compression.decode_file(argv[3], argv[5]);
        } else {
            throw InvalidateArguments();
        }
    } catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}

/* End of 'main.cpp' file */