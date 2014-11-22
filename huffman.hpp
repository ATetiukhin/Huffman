#ifndef HUFFMAN_HPP_INCLUDE
#define HUFFMAN_HPP_INCLUDE

#include <fstream>        // std::ifstream std::ofstream
#include <vector>
#include <unordered_map>

struct INode;

class Huffman {
public:
    Huffman(const char *input, const char *output);

    ~Huffman();

    void file_encode();

    void file_decode();

    void print_data() const;

private:
    typedef std::vector<bool> HuffmanCodeType;
    typedef std::unordered_map<char, HuffmanCodeType> HuffmanTableType;

    std::ifstream infile;
    std::ofstream outfile;

    // table frequencies
    std::vector<int> frequencies;

    // the original size of the data
    size_t size_infile;

    // the resulting size of the data
    size_t size_outfile;

    // the size necessary for storing auxiliary data
    size_t supporting_data;

    bool build_tree(INode *&root) const;

    void build_table(const INode *node, const HuffmanCodeType &prefix, HuffmanTableType &huffman_table) const;

    bool read_encode(std::ifstream &infile);

    void write_encode(std::ifstream &infile, std::ofstream &outfile, HuffmanTableType &huffman_table);

    bool read_decode(std::ifstream &infile);

    void write_decode(std::ifstream &infile, std::ofstream &outfile, INode *root);

};

#endif /* End of 'huffman.hpp' file */