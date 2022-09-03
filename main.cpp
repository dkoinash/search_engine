#include "searchserver.h"

class ConfigMissing: public std::exception
{
public:
    const char* what() const noexcept override
    {
        return "The config file is missing.";
    }
};
class RequestMissing: public std::exception
{
public:
    const char* what() const noexcept override
    {
        return "The request file is missing.";
    }
};
class ConfigEmpty: public std::exception
{
public:
    const char* what() const noexcept override
    {
        return "The config file is empty.";
    }
};
class RequestEmpty: public std::exception
{
public:
    const char* what() const noexcept override
    {
        return "The request file is empty.";
    }
};


void CheckConfigFiles(){
    std::ifstream config, requests;
    config.open("config.json");
    if(!config.is_open()){
        throw ConfigMissing();
    }
    nlohmann::json configJson;
    config >> configJson;
    if(configJson.find("config") == configJson.end() ||
            configJson.find("files") == configJson.end()) throw ConfigEmpty();
    config.close();
    requests.open("requests.json");
    if(!requests.is_open()){
        throw RequestMissing();
    }
    nlohmann::json requestJson;
    requests >> requestJson;
    if(requestJson.find("requests") == requestJson.end()) throw RequestEmpty();
    requests.close();
}

int main()
{
    try{
        CheckConfigFiles();
    }
    catch(const std::exception &ex){
        std::cerr << ex.what() << std::endl;
        return 0;
    }
    ConverterJSON configSearch;


    std::vector<std::string> searchRequest = configSearch.GetRequest();

    InvertedIndex idx;

    idx.UpdateDocumentBase(configSearch.GetTextDocuments());

    std::vector<std::vector<Entry>> results;

    for(auto& request : searchRequest) {
        std::vector<Entry> word_count = idx.GetWordCount(request);
        results.push_back(word_count);
    }

    SearchServer srv(idx);

    std::vector<std::vector<RelativeIndex>> answer = srv.search(searchRequest);

    configSearch.putAnswers(answer);

    return 0;
}
