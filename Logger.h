#pragma once

#include <string>
#include <vector>
#include <unordered_map>

class Logger {
private:
    std::string m_filePath{};

public:
    Logger(const std::string& path) 
        : m_filePath(path) {}

    void saveToFile(const std::vector<std::string>& vecLine);
    void analyzeFile();
    const std::string& getFilePath() { return m_filePath; }
};