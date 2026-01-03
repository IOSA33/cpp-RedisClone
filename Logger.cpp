#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include "Logger.h"
#include "Timer.h"
#include "Redis.h"

void Logger::saveToFile(const std::vector<std::string>& vecLine, Timer& timer) {
    std::ofstream path{ m_filePathAOF, std::ios::app};

    if(!path.is_open()) {
        std::cout << "Error in Logger::saveToFile(): Cannot open a file!\n";
        return;
    }
    for (size_t i = 0; i < vecLine.size(); ++i) {
        if (i == 3) {
            std::string expireTime = vecLine[i];
            path << timer.now() + std::stod(expireTime) << ' ';
        } else {
            path << vecLine[i] << ' ';
        }
    }

    if (vecLine.size() < 4 && vecLine[0] == "SET") {
        path << timer.now() + DefaultValues::expireAfter << ' ';
    }

    path << '\n';

    path.close();
}

void Logger::snapshot_RDB(const std::unordered_map<std::string, PayLoad>& u_map) {
    std::ofstream file { m_filePathSnapshot, std::ios::trunc };
    
    if (!file.is_open()) {
        std::cout << "Error in Logger::snapshot_RDB(): Cannot open a file!\n";
        return;
    }

    for (const auto& line: u_map) {
        file << "SET " << line.first << ' ' << line.second.value << ' ' << line.second.TTL << ' ' << '\n';
    }

    file.close();

    std::ofstream aofFile { m_filePathAOF, std::ios::trunc };
    aofFile.close();
}