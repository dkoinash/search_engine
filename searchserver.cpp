#include "searchserver.h"

std::vector<std::vector<RelativeIndex>> SearchServer::search(const std::vector<std::string>& queries_input)
{
    std::vector<std::vector<RelativeIndex>> result;

    int maxCountResult = 5;
    //ConverterJSON jsonData;
    maxCountResult = ConverterJSON::GetResponsesLimit();

    float maxRel = 0.0;

    for (auto str : queries_input) {

        std::vector<std::string> listWordsToSearch;
        std::string tmpWord;
        for (size_t j = 0; j < str.size(); ++j) {
            bool wrdDone = false;
            if (str[j] != ' ') {
                tmpWord += str[j];
            }
            else {
                wrdDone = true;
            }
            if (wrdDone || j == str.size() - 1) {
                bool testUniq = true;
                for (const auto& wrd : listWordsToSearch) {
                    if (wrd == tmpWord) testUniq = false;
                }
                if (testUniq) listWordsToSearch.push_back(tmpWord);
                tmpWord.clear();
            }
        }

        std::vector<std::map<size_t, float>> vecMapEntry;
        std::map<size_t, float> mapEntry;
        for (auto &word : listWordsToSearch) {
            std::vector<Entry> EntryWord = _index.GetWordCount(word);
            for (auto res : EntryWord) {
                if (mapEntry.find(res.docId) == mapEntry.end()) mapEntry.insert(std::make_pair(res.docId, res.count));
                else mapEntry[res.docId] += (float)res.count;
            }
            vecMapEntry.push_back(mapEntry);
        }

        for (auto vec : vecMapEntry) {
            for (auto it = vec.begin(); it != vec.end(); ++it) {
                if (maxRel < it->second) maxRel = it->second;
            }
        }

        std::map<size_t, float> resMapEntry;
        for (auto &wrdMapEntry : vecMapEntry) {
            for(auto it = wrdMapEntry.begin(); it != wrdMapEntry.end(); ++it){
                if(resMapEntry.find(it->first) == resMapEntry.end()) {
                    resMapEntry.emplace(std::make_pair(it->first, it->second));
                }
                else{
                    if(resMapEntry[it->first] < it->second) resMapEntry[it->first] = it->second;
                }
            }
        }

        std::vector<RelativeIndex > resultPerWord;
        size_t maxId = 0;
        float mRank = 0.0;
        int locCounttoRes = 0;
        while (!resMapEntry.empty()) {
            for (auto & it : resMapEntry) {
                float tmpRank = it.second / maxRel;
                if (tmpRank > mRank) {
                    mRank = it.second / maxRel;
                    maxId = it.first;
                }
            }
            RelativeIndex tmpIdx;
            tmpIdx.docId = maxId;
            tmpIdx.rank = mRank;
            auto to_erase = resMapEntry.find(maxId);
            resMapEntry.erase(to_erase);
            resultPerWord.push_back(tmpIdx);

            maxId = 0;
            mRank = 0.0;
            locCounttoRes++;
            if(locCounttoRes >= maxCountResult) break;
        }
        result.push_back(resultPerWord);
    }
    return result;
}
