#pragma once

#include <unordered_map>
#include <string>
#include <utility>
#include "Timer.h"
#include "Logger.h"

namespace Err{
    enum Type{
        NoError,
        NoSuchKey,
        Expired,
        Max_Err_Type
    };
};

namespace Log{
    enum Type {
        Logging,
        NoLogging,
        Max_Log_Type
    };
};

class Redis {
private:
    Timer m_timer{};
    Logger m_logger;
    std::unordered_map<std::string, std::unordered_map<double, std::string>> m_umap{};
    std::vector<std::string> m_currValidCmd{};

public:
    // Network
    // AnalyzeFile
    Redis(const std::string& pathToFile) : m_logger(pathToFile) {
        readFromFile();
    } 
    ~Redis() {
        //m_logger.analyzeFile();
    }

    void run();
    // Returns response code
    bool parser(const std::string& input);
    std::string executeValidCmd(Log::Type code);
    std::string setValue(const std::string& key, const std::string& value, double exprireAfter = 10.0);
    std::pair<std::string, Err::Type> getValue(const std::string& key) const;
    bool deleteValue(const std::string& key);
    bool isStringDigit(const std::string& input);
    void readFromFile();
    void clearCurrCmd() { m_currValidCmd.clear(); }
};