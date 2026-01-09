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
    constexpr int expireAfter { 100 };
};

struct PayLoad{
    double TTL{};
    std::string value{};
};

class Redis {
private:
    Timer m_timer{};
    Logger m_logger;
    // This hashmap is for session = payload, quick check for existance
    std::unordered_map<std::string, PayLoad> m_umap{};
    // This hashmap contains every user sessions that he has,id = sessions
    // We can delete/revoke them by one command and save only current session
    std::unordered_map<std::string, std::vector<PayLoad>> m_sessions{};
    std::vector<std::string> m_currValidCmd{};

public:
    Redis(const std::string& pathToFileAOF, const std::string& pathToFileRDB) : m_logger(pathToFileAOF, pathToFileRDB) {
        readFromFile( m_logger.getFilePathSnapShot() );
        readFromFile( m_logger.getFilePathAOF() );
    } 
    ~Redis() {
        m_logger.snapshot_RDB(m_umap, m_timer);
    }

    void run();
    // Returns response code
    bool parser(const std::string& input);
    std::string getAll();
    void saveToSnapshot();
    std::string executeValidCmd(Log::Type code);
    bool addToListSessions(const std::string& key, const std::string& value, double exprireAfter);
    std::string getAllSessions(const std::string& key);
    std::string setValue(const std::string& key, const std::string& value, double exprireAfter, Log::Type log = Log::Logging);
    std::pair<std::string, Err::Type> getValue(const std::string& key) const;
    bool deleteValue(const std::string& key);
    bool exists(const std::string& key) const;
    bool isStringDigit(const std::string& input);
    void readFromFile(const std::string& path);
    void clearCurrCmd() { m_currValidCmd.clear(); }
};