#include "converterjson.h"

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
class AnswerMissing: public std::exception
{
public:
    const char* what() const noexcept override
    {
        return "The answer file not created.";
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


void checkConfigFile(){
    std::ifstream configFile;
    configFile.open("config.json");
    nlohmann::json configJson;
    configFile >> configJson;
    if (configJson.find("config") == configJson.end() ||
        configJson.find("files") == configJson.end())
        throw ConfigEmpty();
    configFile.close();
    nlohmann::json configJsonConfig = configJson["config"];
    if (configJsonConfig.find("max_responses") == configJsonConfig.end() ||
        configJsonConfig.find("name") == configJsonConfig.end() ||
        configJsonConfig.find("version") == configJsonConfig.end())
        throw ConfigEmpty();
}

void checkRequestFile(){
    std::ifstream requestFile;
    requestFile.open("requests.json");
    if(!requestFile.is_open()){
        throw RequestMissing();
    }else {
        nlohmann::json requestJson;
        requestFile >> requestJson;
        if (requestJson.find("requests") == requestJson.end()) throw RequestEmpty();
        requestFile.close();
    }
}



std::mutex mtx;
std::string getDoc(const std::string& address) {
    std::ifstream docFile;
    docFile.open(address);
    std::string str;
    std::string doc;

    if(!docFile.is_open()) {
        std::lock_guard<std::mutex> guard(mtx);
        std::cerr << "file with address \"" << address << "\" is missing." << std::endl;
    }
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
        checkConfigFile();
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
    }else throw ConfigMissing();
    return result;
}


int ConverterJSON::GetResponsesLimit() {
    std::ifstream configFile;
    configFile.open("config.json");
    if(configFile.is_open()){
        checkConfigFile();
        nlohmann::json dict;
        configFile >> dict;
        configFile.close();
        nlohmann::json config_part = dict["config"];
        std::cout << "Starting " << config_part["name"] << std::endl;
        std::cout << "version: " << config_part["version"] << std::endl;
        return config_part["max_responses"];
    }else throw ConfigMissing();
    return 5;
}

std::vector<std::string> ConverterJSON::GetRequest() {
    std::vector<std::string> result;
    std::ifstream requestFile;
    requestFile.open("requests.json");
    if(requestFile.is_open()){
        checkRequestFile();
        nlohmann::json list;
        requestFile >> list;
        requestFile.close();
        for(auto it = list["requests"].begin(); it != list["requests"].end(); it++) {
            result.push_back(it.value());
        }
    }else throw RequestMissing();
    return result;
}

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
                tmpPart["docid"] = itReq->docId;
                tmpPart["rank"] = ((int)(itReq->rank * 1000 + .5) / 1000.0);
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
    std::ofstream readyAnswerFile;
    readyAnswerFile.open("answer.json");
    if(readyAnswerFile.is_open()){
        readyAnswerFile << answersMain;
        readyAnswerFile.close();
    }else throw AnswerMissing();
}
