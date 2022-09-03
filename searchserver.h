#pragma once
#include "invertedindex.h"

class SearchServer{
public:
    explicit SearchServer(InvertedIndex& idx) : _index(idx){ };

    std::vector<std::vector<RelativeIndex>> search(const std::vector<std::string>& queries_input);
private:
    InvertedIndex _index;
};