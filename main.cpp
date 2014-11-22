#include <iostream>    // std::cerr
#include <stdexcept>   // std::runtime_error
#include <cstring>     // strcmp
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

void parse_the_command_line(int argc, char *argv[], int &parameter, char *&input_file, char *&output_file) {
    char const *const cmd[] = {"-c", "-u", "-f", "--file", "-o", "--output"};

    if (argc == 6) {
        if ((strcmp(argv[2], cmd[2]) || strcmp(argv[2], cmd[3])) && (strcmp(argv[4], cmd[4]) || strcmp(argv[4], cmd[5]))) {
            input_file = argv[3];
            output_file = argv[5];
            if (!strcmp(argv[1], cmd[0])) {
                parameter = 1;
            } else if (!strcmp(argv[1], cmd[1])) {
                parameter = 2;
            } else {
                throw std::runtime_error("invalidate arguments");
            }
            return;
        }
    }

    throw std::runtime_error("invalidate arguments");
}

int main(int argc, char *argv[]) {
/* Debug memory check support */
#ifdef _DEBUG
    SetDbgMemHooks();
#endif
    std::ios_base::sync_with_stdio(0);

    try {
        int parameter = 0;
        char *input_file = nullptr;
        char *output_file = nullptr;
        parse_the_command_line(argc, argv, parameter, input_file, output_file);

        Huffman compression(input_file, output_file);
        if (parameter == 1) {
            compression.file_encode();
        } else {
            compression.file_decode();
        }
        compression.print_data();

    } catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}

/* End of 'main.cpp' file */