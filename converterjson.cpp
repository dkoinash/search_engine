#include "converterjson.h"

std::string getDoc(const std::string& address) {
    std::ifstream docFile;
    docFile.open(address);
    std::string str;
    std::string doc;
    if(!docFile.is_open()) std::cerr << "file with address \"" << address << "\" is missing." << std::endl;
    else{
        do {
            str.clear();
            docFile >> str;
            if(doc.size() == 0) doc += str;
            else doc += " " + str;
        } while (!docFile.eof());
    }
    return doc;
}

std::vector<std::string> ConverterJSON::GetTextDocuments() {
    std::vector<std::string> result;
    std::ifstream configFile;
    configFile.open("config.json");

    if(configFile.is_open()){
        nlohmann::json file;
        configFile >> file;
        configFile.close();
        nlohmann::json list = file["files"];

        std::queue<std::future<std::string>> futureDocs;
        for(auto it = list.begin(); it != list.end(); it++){
            futureDocs.push(std::async(getDoc, it.value()));
        }

        while (!futureDocs.empty()) {
            futureDocs.front().wait();
            result.push_back(futureDocs.front().get());
            futureDocs.pop();
        }
    }
    return result;
}

//Метод считывает поле max_responses для определения предельного количества ответов на один запрос @return
int ConverterJSON::GetResponsesLimit() {
    std::ifstream configFile;
    configFile.open("config.json");

    if(configFile.is_open()){
        nlohmann::json dict;
        configFile >> dict;
        configFile.close();
        nlohmann::json config_part = dict["config"];
        std::cout << "Starting " << config_part["name"] << std::endl;
        std::cout << "version: " << config_part["version"] << std::endl;
        return config_part["max_responses"];
    }
    return 5;
}

//Метод получения запросов из файла request.json @return возвращает список запросов из файла request.json
std::vector<std::string> ConverterJSON::GetRequest() {
    std::vector<std::string> result;
    std::ifstream requestFile;
    requestFile.open("requests.json");
    if(requestFile.is_open()){
        nlohmann::json list;
        requestFile >> list;
        requestFile.close();
        for(auto it = list["requests"].begin(); it != list["requests"].end(); it++) {
            result.push_back(it.value());
        }
    }
    return result;
}
//*Положить в файл answers.json результаты поисковых запросов
//old//void ConverterJSON::putAnswers(std::vector<std::vector<std::pair<int, float>>> answers) {
void ConverterJSON::putAnswers(std::vector<std::vector<RelativeIndex>> answers) {
    nlohmann::json answersMain;
    nlohmann::json answersSec;
    int numberReq = 0;
    for(auto it = answers.begin(); it != answers.end(); ++it){
        nlohmann::json requestMain;
        nlohmann::json relevanceMain;
        if(!it->empty()){
            requestMain["_result"] = "true";
            for(auto itReq = it->begin(); itReq != it->end(); ++itReq){
                nlohmann::json tmpPart;
                //old//tmpPart["docid"] = itReq->first;
                tmpPart["docid"] = itReq->docId;
                //old//tmpPart["rank"] = itReq->second;
                tmpPart["rank"] = itReq->rank;
                relevanceMain += tmpPart;
            }
            requestMain["relevance"] = relevanceMain;
        }
        else{
            requestMain["_result"] = "false";
        }
        std::string tmpRequest = "request" + std::to_string(numberReq);
        answersSec[tmpRequest] = requestMain;
        numberReq++;
    }
    answersMain["answers"] = answersSec;
    std::ofstream readyAnswerFile("answer.json");
    readyAnswerFile << answersMain;
    readyAnswerFile.close();
}
