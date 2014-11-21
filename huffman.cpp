#include <iostream>  // std::cout
#include <fstream>   // std::ifstream
#include <iterator>  // std::ostream_iterator
#include <algorithm> // std::copy_n
#include <climits>   // CHAR_BIT
#include <queue>
#include "huffman.hpp"

typedef unsigned int uint;
typedef unsigned char uchar;

Huffman::Huffman()
        : unique_symbols(1 << CHAR_BIT),
          frequencies(unique_symbols),
          input_data(0),
          output_data(0),
          supporting_data(0) {
}

Huffman::~Huffman() {
}

void Huffman::print_data() const {
    std::cout << input_data << std::endl;
    std::cout << output_data << std::endl;
    std::cout << supporting_data << std::endl;
}

void Huffman::frequency_count() {
    // build frequency array
    for (std::vector<char>::iterator it = text_buffer.begin(), end = text_buffer.end(); it != end; ++it) {
        ++frequencies[*it];
    }
}

INode *Huffman::build_tree(int &size_tree) const {
    std::priority_queue<INode *, std::vector<INode *>, NodeCmp> trees;
    for (int i = 0; i < unique_symbols; ++i) {
        if (frequencies[i] != 0)
            trees.push(new LeafNode(frequencies[i], static_cast<char>(i)));
    }

    size_tree = trees.size();

    INode *right = nullptr;
    INode *left = nullptr;
    INode *parent = nullptr;
    while (trees.size() > 1) {
        right = trees.top();
        trees.pop();

        left = trees.top();
        trees.pop();

        parent = new InternalNode(right, left);
        trees.push(parent);
    }
    return trees.top();
}

void Huffman::build_table(const INode *node, const HuffmanCodeType &prefix, HuffmanTableType &huffman_table) const {
    if (const LeafNode *lf = dynamic_cast<const LeafNode *>(node)) {
        huffman_table[lf->c] = prefix;
    } else if (const InternalNode *in = dynamic_cast<const InternalNode *>(node)) {
        HuffmanCodeType leftPrefix = prefix;
        leftPrefix.push_back(true);
        build_table(in->left, leftPrefix, huffman_table);

        HuffmanCodeType rightPrefix = prefix;
        rightPrefix.push_back(false);
        build_table(in->right, rightPrefix, huffman_table);
    }
}

bool Huffman::read_encode(const char *input) {
    std::ifstream infile(input, std::ifstream::binary | std::ifstream::out);
    if (!infile.is_open()) {
        throw std::runtime_error("invalidate input file");
    }

    // get size of file
    infile.seekg(0, infile.end);
    input_data = static_cast<int>(infile.tellg());
    if (!input_data) {
        infile.close();
        return false;
    }
    infile.seekg(0);

    // allocate memory for file content
    text_buffer.resize(static_cast<uint>(input_data));

    // read content of infile
    infile.read(text_buffer.data(), input_data);

    infile.close();
    return true;
}

void Huffman::write_encode(const char *output, HuffmanTableType &huffman_table) {
    std::ofstream outfile(output, std::ofstream::binary | std::ofstream::out);
    if (!outfile.is_open()) {
        throw std::runtime_error("invalidate output file");
    }

    // write frequencies array
    short count = static_cast<short>(huffman_table.size());
    outfile.write((char *)&count, sizeof(short));

    for (int idx = 0; idx < unique_symbols; ++idx) {
        if (frequencies[idx]) {
            outfile.write((char *) &idx, sizeof(char));
            outfile.write((char *) &frequencies[idx], sizeof(int));
            supporting_data += 1;
        }
    }

    supporting_data = sizeof(short) + supporting_data * (sizeof(int) + sizeof(char));

    // write text
    std::vector<bool> huffman_text;
    huffman_text.reserve(6 * 8 * 1024 * 1024);

    HuffmanTableType::iterator huffman_iter;
    for (std::vector<char>::iterator itr = text_buffer.begin(), end = text_buffer.end(); itr != end; ++itr) {
        huffman_iter = huffman_table.find(*itr);
        for (bool bit : huffman_iter->second) {
            huffman_text.push_back(bit);
        }
    }

    int size = static_cast<int>(huffman_text.size());
    outfile.write((char*)&size, sizeof(int));

    outfile.write((char*)&huffman_text[0], huffman_text.size());

    // supporting_data + S()
    output_data = supporting_data + sizeof(int) + size;
    outfile.close();
}

void Huffman::file_encode(const char *input, const char *output) {
    if (read_encode(input)) {
        frequency_count();
        int size_tree = 0;
        INode *root = build_tree(size_tree);

        HuffmanTableType huffman_table;
        if (size_tree == 1) {
            build_table(root, HuffmanCodeType(1), huffman_table);
        } else {
            build_table(root, HuffmanCodeType(0), huffman_table);
        }
        delete root;

        write_encode(output, huffman_table);
    }

    print_data();
}

bool Huffman::read_decode(const char *input) {
    std::ifstream infile(input, std::ifstream::binary | std::ifstream::out);
    if (!infile.is_open()) {
        throw std::runtime_error("invalidate input file");
    }

    // get size of file
    infile.seekg(0, infile.end);
    input_data = static_cast<int>(infile.tellg());
    infile.seekg(0);

    if (!input_data) {
        infile.close();
        return false;
    }

    infile.close();
    return true;
}

void Huffman::write_decode(const char *output, INode *root) {
    std::ofstream outfile(output, std::ofstream::binary | std::ofstream::out);
    if (!outfile.is_open()) {
        throw std::runtime_error("invalidate output file");
    }
    outfile.close();
}

void Huffman::file_decode(const char *input, const char *output) {
    if (read_decode(input)) {
        int size_tree = 0;
        INode *root = build_tree(size_tree);

        write_decode(output, root);
        delete root;
    }
    print_data();
}

/* End of 'huffman.cpp' file */