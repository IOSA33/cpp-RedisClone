#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include "Logger.h"

void Logger::saveToFile(const std::vector<std::string>& vecLine) {
    std::ofstream path{ m_filePath, std::ios::app};

    if(!path.is_open()) {
        std::cout << "Error in Logger::saveToFile(): Cannot open a file!\n";
        return;
    }
    for (const auto& i: vecLine) {
        path << i << ' ';
    }

    path << '\n';

    path.close();
}

void Logger::analyzeFile() {
    std::ifstream file { m_filePath };

    if (!file.is_open()) {
        std::cout << "Error in Logger::analyzeFile(): Cannot open a file!\n";
        return;
    }

    std::string inputLine{};
    

    while(std::getline(file, inputLine)) {
        
    }
}