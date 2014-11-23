#include <iostream>  // std::cout
#include <fstream>   // std::ifstream std::ofstream
#include <queue>
#include <bitset>
#include "huffman.hpp"

#define CHAR_BIT 8 // number of bits in a char

typedef unsigned char uchar_t;
typedef unsigned short ushort_t;
typedef unsigned long ulong_t;

struct INode {
    const int f;

    virtual ~INode() {
    }

protected:
    INode(int f) : f(f) {
    }
};

struct InternalNode : public INode {
    INode *const left;
    INode *const right;

    InternalNode(INode *c0, INode *c1) : INode(c0->f + c1->f), left(c0), right(c1) {
    }

    ~InternalNode() {
        delete left;
        delete right;
    }
};

struct LeafNode : public INode {
    const uchar_t c;

    LeafNode(int f, uchar_t c) : INode(f), c(c) {
    }
};

struct NodeCmp {
    bool operator()(const INode *lhs, const INode *rhs) const {
        return lhs->f > rhs->f;
    }
};

inline void get_size_of_file(std::ifstream &filename, size_t &size_data) {
    filename.seekg(0, filename.end);
    size_data = static_cast<size_t>(filename.tellg());
    filename.seekg(0);
}

Huffman::Huffman(const char *input, const char *output) :
        infile(input, std::ifstream::binary | std::ifstream::out),
        outfile(output, std::ofstream::binary | std::ofstream::out),
        frequencies(1 << CHAR_BIT),
        size_infile(0),
        size_outfile(0),
        supporting_data(0) {
}

Huffman::~Huffman() {
    infile.close();
    outfile.close();
}

void Huffman::print_data() const {
    std::cout << size_infile << std::endl;
    std::cout << size_outfile << std::endl;
    std::cout << supporting_data << std::endl;
}

bool Huffman::build_tree(INode *&root) const {
    std::priority_queue<INode *, std::vector<INode *>, NodeCmp> trees;

    for (int i = 0, size = frequencies.size(); i < size; ++i) {
        if (frequencies[i] != 0)
            trees.push(new LeafNode(frequencies[i], static_cast<uchar_t>(i)));
    }

    bool isOne = (trees.size() == 1);

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

    root = trees.top();
    return isOne;
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

void Huffman::file_encode() {

    if (!(infile.is_open() && outfile.is_open())) {
        throw std::runtime_error("invalidate file");
    }

    if (read_encode(infile)) {
        INode *root = nullptr;
        HuffmanTableType huffman_table;

        if (build_tree(root)) {
            build_table(root, HuffmanCodeType(1), huffman_table);
        } else {
            build_table(root, HuffmanCodeType(0), huffman_table);
        }
        delete root;

        write_encode(infile, outfile, huffman_table);
    }
}

bool Huffman::read_encode(std::ifstream &infile) {
    get_size_of_file(infile, size_infile);
    if (!size_infile) {
        return false;
    }

    // read content of input file
    uchar_t ch = 0;
    for (size_t i = 0; i < size_infile; ++i) {
        infile.read(reinterpret_cast<char *>(&ch), sizeof(char));
        ++frequencies[ch];
    }

    infile.seekg(0);

    return true;
}

void Huffman::write_encode(std::ifstream &infile, std::ofstream &outfile, HuffmanTableType &huffman_table) {
    /* write frequencies table */
    ushort_t size_huffman_table = static_cast<ushort_t>(huffman_table.size());
    outfile.write(reinterpret_cast<char *>(&size_huffman_table), sizeof(short));

    for (int idx = 0, size = frequencies.size(); idx < size; ++idx) {
        if (frequencies[idx]) {
            outfile.write(reinterpret_cast<char *>(&idx), sizeof(char));
            outfile.write(reinterpret_cast<char *>(&frequencies[idx]), sizeof(int));
            supporting_data += 1;
        }
    }

    // size supporting data
    supporting_data = sizeof(short) + supporting_data * (sizeof(int) + sizeof(char));

    /* write text */
    const int size_ulong = 32;
    ulong_t number_for_write = 0;

    std::vector<bool> bits_text;
    bits_text.reserve(6 * 8 * 1024 * 1024);

    // fill bits_text
    char ch = 0;
    HuffmanTableType::iterator iter_table;
    for (size_t i = 0; i < size_infile; ++i) {
        infile.read(&ch, sizeof(char));
        iter_table = huffman_table.find(ch);
        for (bool bit : iter_table->second) {
            bits_text.push_back(bit);
        }
    }

    ulong_t size_bits_text = static_cast<ulong_t>(bits_text.size());
    outfile.write(reinterpret_cast<char *>(&size_bits_text), sizeof(ulong_t));

    int count = 0;
    for (ulong_t i = 0, j = 0; i != size_bits_text;) {
        std::bitset<size_ulong> ulong;
        for (j = 0; j != size_ulong && i != size_bits_text; ++j, ++i) {
            ulong[j] = bits_text[i];
        }
        number_for_write = ulong.to_ulong();
        outfile.write(reinterpret_cast<char *>(&number_for_write), sizeof(ulong_t));
        ++count;
    }

    size_outfile = supporting_data + sizeof(ulong_t) + count * sizeof(ulong_t);
}

void Huffman::file_decode() {
    if (!(infile.is_open() && outfile.is_open())) {
        throw std::runtime_error("invalidate file");
    }

    if (read_decode(infile)) {
        INode *root = nullptr;
        build_tree(root);
        write_decode(infile, outfile, root);
        delete root;
    }
}

bool Huffman::read_decode(std::ifstream &infile) {
    get_size_of_file(infile, size_infile);
    if (!size_infile) {
        return false;
    }

    /* read frequencies table */
    ushort_t size_huffman_table = 0;
    infile.read(reinterpret_cast<char *>(&size_huffman_table), sizeof(short));

    uchar_t ch = 0;
    int count = 0;
    for (ushort_t idx = 0; idx < size_huffman_table; ++idx) {
        infile.read(reinterpret_cast<char *>(&ch), sizeof(char));
        infile.read(reinterpret_cast<char *>(&count), sizeof(int));

        frequencies[ch] = count;
        supporting_data += 1;
    }

    // size supporting data
    supporting_data = sizeof(short) + supporting_data * (sizeof(int) + sizeof(char));

    return true;
}

void Huffman::write_decode(std::ifstream &infile, std::ofstream &outfile, INode *root) {
    const int size_ulong = 32;
    ulong_t number_for_write = 0;

    std::vector<bool> bits_text;
    bits_text.reserve(6 * 8 * 1024 * 1024);

    ulong_t size_bits_text = static_cast<ulong_t>(bits_text.size());
    infile.read(reinterpret_cast<char *>(&size_bits_text), sizeof(ulong_t));

    for (ulong_t i = 0, j = 0; i != size_bits_text;) {
        infile.read(reinterpret_cast<char *>(&number_for_write), sizeof(ulong_t));
        std::bitset<size_ulong> ulong(number_for_write);
        for (j = 0; j != size_ulong && i != size_bits_text; ++j, ++i) {
            bits_text.push_back(ulong[j]);
        }
    }

    INode *node = root;
    int count = 0;
    for (std::vector<bool>::iterator it = bits_text.begin(), end = bits_text.end(); it < end;) {
        if (const LeafNode *lf = dynamic_cast<const LeafNode *>(node)) {
            outfile.write(reinterpret_cast<const char *>(&(lf->c)), sizeof(char));
            node = root;
            ++count;
            ++it;
        } else if (const InternalNode *in = dynamic_cast<const InternalNode *>(node)) {
            node = *it ? in->left : in->right;

            if (dynamic_cast<const InternalNode *>(node)) {
                ++it;
            }
        }
    }

    size_outfile = count * sizeof(char);
}

/* End of 'huffman.cpp' file */