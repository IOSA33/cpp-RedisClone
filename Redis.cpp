#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <fstream>
#include "Redis.h"
#include "NetWork/Server.h"

// Commands:
// SET [key] [value] *[expire] : returns "OK"
// GET [key] : returns "value"
// DELETE [key] : returns "Deleted Successfully!"
// SAVE : saves explicitly logs to the file
// exit : exists the program

// For local run without Server
void Redis::run() {
    while(true) {
        std::cout << "\nWrite a command: \n";
        std::cout << "1: SET [key] [value] *[expire]\n";
        std::cout << "2: GET [key]\n";
        std::cout << "3: DELETE [key]\n";

        std::string input{};
        std::getline(std::cin >> std::ws, input);

        // If input is correct
        if (parser(input)) {
            const std::string response { executeValidCmd(Log::Logging) };
            if (response == "exit") {
                std::cout << "See you and Happy life :)" << std::endl;
                return;
            }
            std::cout << response << '\n';
            m_currValidCmd.clear();
        } else {
            m_currValidCmd.clear();
        }
    }
}

std::string Redis::executeValidCmd(Log::Type code) {
    if (m_currValidCmd[0] == "exit") {
        std::cout << "\n";
        return "exit";
    }

    if (m_currValidCmd[0] == "SET") {
        if (code == Log::Logging)
            std::cout << "\n";
        if (m_currValidCmd.size() == 4) {
            if (isStringDigit(m_currValidCmd[3])) {
                if (code == Log::Logging) {
                    m_logger.saveToFile(m_currValidCmd, m_timer);
                    return setValue(m_currValidCmd[1], m_currValidCmd[2], std::stod(m_currValidCmd[3]));
                } 

                return setValue(m_currValidCmd[1], m_currValidCmd[2], std::stod(m_currValidCmd[3]), Log::NoLogging);
            }
        } else {
            if (code == Log::Logging) {
                m_logger.saveToFile(m_currValidCmd, m_timer);
                return setValue(m_currValidCmd[1], m_currValidCmd[2], DefaultValues::expireAfter);
            }
            
            return setValue(m_currValidCmd[1], m_currValidCmd[2], DefaultValues::expireAfter, Log::NoLogging);
        }
    }

    if (m_currValidCmd[0] == "GET") {
        std::cout << "\n";
        auto result { getValue(m_currValidCmd[1]) };
        if (result.second == Err::NoError) {
            return result.first;
        } else {
            if (result.second == Err::NoSuchKey) {
                return result.first;
            } else {
                deleteValue(m_currValidCmd[1]);
                return result.first;
            }
        }
    }

    if (m_currValidCmd[0] == "DELETE") {
        if (code == Log::Logging)
            std::cout << "\n";
        if (deleteValue(m_currValidCmd[1])) {
            if (code == Log::Logging)
                m_logger.saveToFile(m_currValidCmd, m_timer);

            return "Deleted Successfully!";
        } else {
            return "Error in Redis::executeValidCmd()/deleteValue(): Cannot delete a key!";
        }
        return "DELETE has less than a 2 arguments";
    }

    return "Error in Redis::executeValidCmd(): unknown Command";
}

bool Redis::parser(const std::string& s) {
    std::vector<std::string> vec{};
    std::string word{};

    for (const auto& i: s) {
        if (i != ' ') {
            word.push_back(i);
            continue;
        }
        if (!word.empty()) {
            vec.emplace_back(word);
            word.clear();
        }
    }

    if (!word.empty()) {
        vec.emplace_back(word);
    }

    if (vec[0] == "exit") {
        m_currValidCmd.emplace_back(vec[0]);
        return true;
    }

    if (vec[0] == "SET") {
        if (vec.size() > 2 && vec.size() < 5) {
            if (vec.size() == 4) {
                if (isStringDigit(vec[3])) {
                    for (size_t valid = 0; valid < 4; ++valid) {
                        m_currValidCmd.emplace_back(vec[valid]);
                    }
                    return true;
                } else {
                    std::cout << "ExpireTime is not a digit!" << '\n';
                    return false;
                }
            } else {
                for (size_t valid = 0; valid < 3; ++valid) {
                    m_currValidCmd.emplace_back(vec[valid]);
                }
                return true;
            }
        } else {
            std::cout << "SET has less/more than a 3-4 arguments" << '\n';
            return false;
        }
    }
    if (vec[0] == "GET") {
        if (vec.size() == 2) {
            for (size_t valid = 0; valid < 2; ++valid) {
                m_currValidCmd.emplace_back(vec[valid]);
            }
            return true;
        } else {
            std::cout << "GET has less/more than a 2 arguments" << '\n';
            return false;
        }
    }
    if (vec[0] == "DELETE") {
        if (vec.size() == 2) {
            for (size_t valid = 0; valid < 2; ++valid) {
                m_currValidCmd.emplace_back(vec[valid]);
            }
            return true;
        } else {
            std::cout << "DELETE has less/more than a 2 arguments" << '\n';
            return false;
        }
    }
    std::cout << "Error in Redis::parser(): unknown Command" << '\n';
    return false;
}

bool Redis::deleteValue(const std::string& key) {
    auto result { m_umap.erase(key) };
    if (result) {
        return true;
    } else {
        return false;
    }
}

std::string Redis::setValue(const std::string& key, const std::string& value, double exprireAfter, Log::Type log) {
    std::pair<std::unordered_map<std::string, PayLoad>::iterator, bool> it{};
    
    // First if, only for if we read from file, we are setting already exists time 
    if (log == Log::Type::NoLogging) {
        it = m_umap.insert({key, {exprireAfter, value}});
    } else {
        // If we do logging, it means that the app is running and we need to set expiretime from now.
        it = m_umap.insert({key, {m_timer.now() + exprireAfter, value}});
    }

    if (it.second == true) {
        return "OK";
    } else {
        auto it { m_umap.find(key) };
        if (it != m_umap.end()) {
            it->second.TTL = m_timer.now() + exprireAfter;
            it->second.value = value;
        }
        return "Overrided: OK";
    }
}

std::pair<std::string, Err::Type> Redis::getValue(const std::string& key) const {
    auto it { m_umap.find(key) };
    if (it != m_umap.end()) {
        std::cout << it->second.TTL << " : now = " << m_timer.now() << '\n'; 
        if (it->second.TTL > m_timer.now()) {
            return std::make_pair(it->second.value, Err::NoError);
        } else {
            return std::make_pair("Key is expired!", Err::Expired);
        }
    }
    return std::make_pair("Error in Redis::getValue(): no such Key", Err::NoSuchKey);
}

// Just for knowledge
bool isdigit(char c) {
    if (c - '0' > 9) {
        return false;
    }
    return true;
}

bool Redis::isStringDigit(const std::string& input) {
    for (const auto& c: input) {
        if (!isdigit(c)){
            return false;
        }
    }
    return true;
}


bool file_is_empty(std::ifstream& file) {
    return file.tellg() == 0 && file.peek() == std::ifstream::traits_type::eof();
}

void Redis::readFromFile(const std::string& path) {
    std::ifstream file { path };

    if (!file.is_open()) {
        std::cout << "Error in Logger::readFromFile(): " << path << ", Cannot open a file!\n";
        return;
    }

    std::string inputLine{};

    if(file_is_empty(file)) {
        std::cout << "Redis::readFromFile(): " << path << ", file is Empty\n";
    } else {
        std::cout << "Redis::readFromFile(): " << path << ", doing restore" << std::endl;
        while (std::getline(file, inputLine)) {
            if (parser(inputLine)) {
                executeValidCmd(Log::NoLogging);
            }
            m_currValidCmd.clear();
        }
    }
}

// Pretty cool :)