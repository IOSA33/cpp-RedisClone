#pragma once

#include <string>
#include <vector>
#include <unordered_map>

class Timer;
struct PayLoad;

class Logger {
private:
    std::string m_filePathAOF{};
    std::string m_filePathSnapshot{};

public:
    Logger(const std::string& pathAOF, const std::string& pathSNAP) 
        : m_filePathAOF(pathAOF), m_filePathSnapshot(pathSNAP) {}

    void saveToFile(const std::vector<std::string>& vecLine, Timer& timer);
    void analyzeFile_AOF();
    void snapshot_RDB(const std::unordered_map<std::string, PayLoad>& u_map);
    const std::string& getFilePathAOF() { return m_filePathAOF; }
    const std::string& getFilePathSnapShot() { return m_filePathSnapshot; }
};