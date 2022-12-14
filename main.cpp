#include "searchserver.h"

int main()
{
    std::vector<std::string> searchRequest;
    try{    searchRequest = ConverterJSON::GetRequest();   }
    catch (nlohmann::json::parse_error &e)
    {
        std::cerr << "File error in request.json. Details: " << e.what() << std::endl;
        return 0;
    }
    catch(const std::exception &ex){
        std::cerr << ex.what() << std::endl;
        return 0;
    }

    InvertedIndex idx;

    try{ idx.UpdateDocumentBase(ConverterJSON::GetTextDocuments()); }
    catch (nlohmann::json::parse_error &e)
    {
        std::cerr << "File error in config.json. Details: " << e.what() << std::endl;
        return 0;
    }
    catch(const std::exception &ex){
        std::cerr << ex.what() << std::endl;
        return 0;
    }

    std::vector<std::vector<Entry>> results;

    for(auto& request : searchRequest) {
        std::vector<Entry> word_count = idx.GetWordCount(request);
        results.push_back(word_count);
    }

    SearchServer srv(idx);

    std::vector<std::vector<RelativeIndex>> answer = srv.search(searchRequest);

    ConverterJSON::putAnswers(answer);

    return 0;
}
