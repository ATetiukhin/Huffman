#include <iostream>  // std::cout
#include <fstream>   // std::ifstream
#include <iterator>  // std::ostream_iterator
#include <algorithm> // std::copy_n
#include <climits>   // CHAR_BIT
#include <queue>
#include "huffman.hpp"

typedef unsigned int uint;

/*
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
*/
Huffman::Huffman()
        : unique_symbols(1 << CHAR_BIT),
          frequencies(static_cast<uint>(unique_symbols)),
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
        leftPrefix.push_back(false);
        build_table(in->left, leftPrefix, huffman_table);

        HuffmanCodeType rightPrefix = prefix;
        rightPrefix.push_back(true);
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
    infile.seekg(0);

    if (!input_data) {
        infile.close();
        return false;
    }

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

    short count = static_cast<short>(huffman_table.size());
    outfile.write((char *) &count, sizeof(short));
    supporting_data += sizeof(short);

    // write frequencies array
    for (int idx = 0; idx < unique_symbols; ++idx) {
        if (frequencies[idx]) {
            outfile.write((char *) &idx, sizeof(char));
            outfile.write((char *) &frequencies[idx], sizeof(int));
            supporting_data += 1;
        }
    }
    supporting_data = supporting_data * (sizeof(int) + sizeof(char));
    output_data += supporting_data;

    // write text
    size_t size = 0;
    HuffmanTableType::iterator huffman_iter;
    for (std::vector<char>::iterator itr = text_buffer.begin(), end = text_buffer.end(); itr != end; ++itr) {
        huffman_iter = huffman_table.find(*itr);
        size = huffman_iter->second.size();
        outfile.write(huffman_iter->second.data(), size * sizeof(char));
        output_data += size;
    }
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
    root;
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