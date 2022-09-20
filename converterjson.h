#pragma once
#include <iostream>
#include <fstream>
#include "nlohmann/json.hpp"
#include <vector>
#include <string>
#include <exception>
#include <cstddef> //size_t
#include <map>
#include <future>
#include <queue>
#include <mutex>

const std::string CONFIG = "config.json";
const std::string REQUEST = "requests.json";

struct Entry {
    size_t docId, count;
    bool operator ==(const Entry& other) const {
        return (docId == other.docId &&
                count == other.count);
    }
};

struct RelativeIndex{
    size_t docId;
    float rank;
    bool operator ==(const RelativeIndex& other) const {
        return (docId == other.docId && rank == other.rank);
    }
};

class ConverterJSON{
public:
    ConverterJSON() = default;

    static std::vector<std::string> GetTextDocuments();

    static int GetResponsesLimit();

    static std::vector<std::string> GetRequest();

    static void putAnswers(std::vector<std::vector<RelativeIndex>> answers);
};