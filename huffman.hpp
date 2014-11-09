#ifndef HUFFMAN_HPP_INCLUDE
#define HUFFMAN_HPP_INCLUDE

#include <vector>
#include <list>
#include <map>

class InvalidateInputFile : public std::exception {
    virtual const char *what() const throw() {
        return "invalidate input file";
    }
};

class InvalidateOutputFile : public std::exception {
    virtual const char *what() const throw() {
        return "invalidate output file";
    }
};

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
    Huffman() {
    }

    ~Huffman() {
    }

    void encode_file(const char *input, const char *output);

    void decode_file(const char *input, const char *output);

private:
    typedef std::vector<char> HuffmanCodeType;
    typedef std::map<char, HuffmanCodeType> HuffmanTableType;
    HuffmanTableType huffman_table;
    std::vector<char> text_buffer;

    int read_file_encode(const char *input);

    int write_file_encode(const char *output);

    INode *build_tree();

    void build_table(const INode *node, const HuffmanCodeType &prefix);

};

#endif /* End of 'huffman.hpp' file */