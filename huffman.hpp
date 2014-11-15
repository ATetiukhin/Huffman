#ifndef HUFFMAN_HPP_INCLUDE
#define HUFFMAN_HPP_INCLUDE

#include <vector>
#include <list>
#include <map>

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
    const char c;

    LeafNode(int f, char c) : INode(f), c(c) {
    }
};

struct NodeCmp {
    bool operator()(const INode *lhs, const INode *rhs) const {
        return lhs->f > rhs->f;
    }
};

class Huffman {
public:
    Huffman();

    ~Huffman();

    void file_encode(const char *input, const char *output);

    void file_decode(const char *input, const char *output);

private:
    typedef std::vector<char> HuffmanCodeType;
    typedef std::map<char, HuffmanCodeType> HuffmanTableType;

    std::vector<char> text_buffer;

    const int unique_symbols;
    std::vector<int> frequencies;

    // the original size of the data
    int input_data;

    // the resulting size of the data
    int output_data;

    // the size necessary for storing auxiliary data
    int supporting_data;

    void print_data() const;

    void frequency_count();

    INode *build_tree(int &size_tree) const;

    void build_table(const INode *node, const HuffmanCodeType &prefix, HuffmanTableType & huffman_table) const;

    bool read_encode(const char *input);

    void write_encode(const char *output, HuffmanTableType & huffman_table);

    bool read_decode(const char *input);

    void write_decode(const char *output, INode *root);

};

#endif /* End of 'huffman.hpp' file */