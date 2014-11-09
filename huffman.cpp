#include <iostream>  // std::cout
#include <fstream>   // std::ifstream
#include <iterator>  // std::ostream_iterator
#include <algorithm> // std::copy_n
#include <queue>

#include "huffman.hpp"

typedef unsigned int uint;

using std::istream;
using std::ostream;

template<typename T>
class wrap {
    T t;

public:
    wrap() : t() {
    }

    wrap(T const &t) : t(t) {
    }

    operator T &() {
        return t;
    }

    operator T const &() const {
        return t;
    }
};

template<typename T>
istream &operator>>(istream &is, wrap<T> &wt) {
    is.read(reinterpret_cast<char *>(&static_cast<T &>(wt)), sizeof(T));
    return is;
}

template<typename T>
ostream &operator<<(ostream &os, wrap<T> const &wt) {
    os.write(
            reinterpret_cast<char const *>(&static_cast<T const &>(wt)),
            sizeof(T));
    return os;
}

void Huffman::encode_file(const char *input, const char *output) {
    // the original size of the data
    std::cout << read_file_encode(input) << std::endl;

    INode *root = build_tree();
    build_table(root, HuffmanCodeType());
    delete root;

    // the resulting size of the data
    std::cout << write_file_encode(output) << std::endl;

    // the size necessary for storing auxiliary data
    std::cout << sizeof(HuffmanTableType) * huffman_table.size() << std::endl;
}

void Huffman::decode_file(const char *input, const char *output) {
    // the original size of the data
    std::ifstream infile(input, std::ifstream::binary | std::ifstream::out);
    if (!infile.is_open()) {
        throw InvalidateInputFile();
    }

    // get size of file
    infile.seekg(0, infile.end);
    int size_input_file = static_cast<int>(infile.tellg());
    infile.seekg(0);
    std::cout << size_input_file << std::endl;

    int count = 0;
    std::copy_n(std::istream_iterator<wrap<int> >(infile), 1, &count);

    HuffmanCodeType code;
    std::map<HuffmanCodeType, char> text;
    char ch = 0;
    char c = 0;
    int i = 0;

    for (i = 0; i < count;) {
        std::copy_n(std::istream_iterator<wrap<char> >(infile), 1, &ch);

        if (ch == '\0' || ch == '\1') {
            code.push_back(ch);
        } else {
            ++i;
            c = ch;
            text[code] = c;
            code.clear();
        }
    }

    std::ofstream outfile(output, std::ofstream::out);
    if (!outfile.is_open()) {
        throw InvalidateInputFile();
    }

    int size = 0;
    std::copy_n(std::istream_iterator<wrap<int> >(infile), 1, &size);
    std::cout << size << std::endl;
    c = '\1';

    count = size_input_file - static_cast<int>(infile.tellg());
    while (outfile.eof()) {
        std::copy_n(std::istream_iterator<wrap<char> >(infile), 1, &ch);

        if (c == '\0' && ch == '\1') {
            outfile << text[code];
            ++size;
            code.clear();
        } else {
            c = ch;
            code.push_back(ch);
        }
    }

    //std::cout << sizeof(std::map<HuffmanCodeType, char>) * text.size() << std::endl;
}

int Huffman::read_file_encode(const char *input) {
    std::ifstream infile(input, std::ifstream::binary | std::ifstream::out);
    if (!infile.is_open()) {
        throw InvalidateInputFile();
    }

    // get size of file
    infile.seekg(0, infile.end);
    int size = static_cast<int>(infile.tellg());
    infile.seekg(0);

    // allocate memory for file content
    text_buffer.resize(static_cast<uint>(size));

    // read content of infile
    infile.read(text_buffer.data(), size);

    infile.close();
    return size;
}

int Huffman::write_file_encode(const char *output) {
    std::ofstream outfile(output, std::ofstream::binary | std::ofstream::out);
    if (!outfile.is_open()) {
        throw InvalidateInputFile();
    }

    size_t count = huffman_table.size();
    std::copy_n(&count, 1, std::ostream_iterator<wrap<size_t> >(outfile));
    // size file
    int size = sizeof(size_t);

    // write table
    for (auto itr = huffman_table.begin(), end = huffman_table.end(); itr != end; ++itr) {
        std::copy(itr->second.begin(), itr->second.end(), std::ostream_iterator<wrap<bool> >(outfile));
        std::copy_n(&(itr->first), 1, std::ostream_iterator<wrap<char> >(outfile));
        size += (1 + (itr->second.end() - itr->second.begin())) * sizeof(char);
    }

    // write text
    count = text_buffer.size();
    std::copy_n(&count, 1, std::ostream_iterator<wrap<size_t> >(outfile));
    size += sizeof(size_t);
    for (auto itr = text_buffer.begin(), end = text_buffer.end(); itr != end; ++itr) {
        copy(huffman_table[*itr].begin(), huffman_table[*itr].end(), std::ostream_iterator<wrap<bool> >(outfile));
        size += (huffman_table[*itr].end() - huffman_table[*itr].begin()) * sizeof(char);
    }

    outfile.close();
    return size;
}

INode *Huffman::build_tree() {
    // build frequency table
    std::map<char, int> frequencies;
    for (auto itr = text_buffer.begin(), end = text_buffer.end(); itr != end; ++itr) {
        frequencies[*itr] += 1;
    }

    int i = 0;
    std::priority_queue<INode *, std::vector<INode *>, NodeCmp> trees;
    for (std::map<char, int>::iterator itr = frequencies.begin(), end = frequencies.end(); itr != end; ++itr, ++i) {
        trees.push(new LeafNode(itr->second, itr->first));
    }

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

void Huffman::build_table(const INode *node, const HuffmanCodeType &prefix) {
    if (const LeafNode *lf = dynamic_cast<const LeafNode *>(node)) {
        huffman_table[lf->c] = prefix;
    } else if (const InternalNode *in = dynamic_cast<const InternalNode *>(node)) {
        HuffmanCodeType leftPrefix = prefix;
        leftPrefix.push_back(false);
        build_table(in->left, leftPrefix);

        HuffmanCodeType rightPrefix = prefix;
        rightPrefix.push_back(true);
        build_table(in->right, rightPrefix);
    }
}

/* End of 'huffman.cpp' file */