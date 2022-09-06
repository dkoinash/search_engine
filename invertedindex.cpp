#include "invertedindex.h"

void InvertedIndex::UpdateDocumentBase(std::vector<std::string> input_docs) {
    for (auto &doc: input_docs) docs.push_back(doc);

    std::vector<std::map<std::string, int>> vecUniWordsCount;
    for(int docNum = 0; docNum < docs.size(); ++docNum){
        std::string tmpWrd;
        std::map<std::string, int> uniWordsCount;
        bool wrdDone = false;
        for(int i = 0; i < docs[docNum].size(); ++i){
            if(docs[docNum][i] != ' ') tmpWrd += docs[docNum][i];
            else wrdDone = true;
            if(wrdDone || i == docs[docNum].size() - 1){
                if(uniWordsCount.find(tmpWrd) == uniWordsCount.end())
                    uniWordsCount.emplace(std::make_pair(tmpWrd,1));
                else
                    uniWordsCount[tmpWrd] += 1;
                wrdDone = false;
                tmpWrd.clear();
            }
        }
        vecUniWordsCount.push_back(uniWordsCount);
    }

    std::vector<std::string> vecCommWords;
    for(int docNum = 0; docNum < docs.size(); ++docNum){
        std::string tmpWrd;
        std::map<std::string, int> uniWordsCount;
        bool wrdDone = false;
        for(int i = 0; i < docs[docNum].size(); ++i){
            if(docs[docNum][i] != ' ') tmpWrd += docs[docNum][i];
            else wrdDone = true;
            if(wrdDone || i == docs[docNum].size() - 1){
                bool is_uniqe = true;
                for(auto &wrd:vecCommWords){
                    if(wrd == tmpWrd) is_uniqe = false;
                }
                if(is_uniqe) vecCommWords.push_back(tmpWrd);
                wrdDone = false;
                tmpWrd.clear();
            }
        }
    }
    for(auto &wrd:vecCommWords){
        size_t wrdCount = 0;
        size_t numDoc = 0;
        std::vector<Entry>vecEntry;
        for (auto& uniWordsCount:vecUniWordsCount){
            for(auto it = uniWordsCount.begin(); it != uniWordsCount.end(); ++it){
                if(wrd == it->first) wrdCount += it->second;
            }
            Entry tmpEntry{};
            tmpEntry.docId = numDoc;
            tmpEntry.count = wrdCount;
            if(wrdCount) vecEntry.push_back(tmpEntry);
            numDoc++;
            wrdCount = 0;
        }
        freq_dictionary.emplace(wrd,vecEntry);
    }
}

std::vector<Entry> InvertedIndex::GetWordCount(const std::string &word) const {
    std::vector<Entry> result;
    auto it = freq_dictionary.find(word);
    if(it != freq_dictionary.end()){
         result = it->second;
    }
    return result;
};


