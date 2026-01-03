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

namespace DefaultValues{
    enum Type {
        expireAfter = 100
    };
};

struct PayLoad{
    double TTL{};
    std::string value{};
};

class Redis {
private:
    Timer m_timer{};
    Logger m_logger;
    std::unordered_map<std::string, PayLoad> m_umap{};
    std::vector<std::string> m_currValidCmd{};

public:
    Redis(const std::string& pathToFileAOF, const std::string& pathToFileRDB) : m_logger(pathToFileAOF, pathToFileRDB) {
        readFromFile( m_logger.getFilePathSnapShot() );
        readFromFile( m_logger.getFilePathAOF() );
    } 
    ~Redis() {
        m_logger.snapshot_RDB(m_umap);
    }

    void run();
    // Returns response code
    bool parser(const std::string& input);
    std::string executeValidCmd(Log::Type code);
    std::string setValue(const std::string& key, const std::string& value, double exprireAfter, Log::Type log = Log::Logging);
    std::pair<std::string, Err::Type> getValue(const std::string& key) const;
    bool deleteValue(const std::string& key);
    bool isStringDigit(const std::string& input);
    void readFromFile(const std::string& path);
    void clearCurrCmd() { m_currValidCmd.clear(); }
};