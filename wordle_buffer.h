#pragma once

#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <map>

class WordleBuffer {
public:
    void write(const std::string& str, size_t idx, bool newline=false) {
        std::lock_guard<std::mutex> lck(m_mtx);
        //*m_buf << str;
        //m_str += str;
        if (m_map.find(idx) == m_map.end()) {
            if (newline) {
                m_map[idx] = str + "\n";
            } else {
                m_map[idx] = str;
            }
        } else {
            if (newline) {
                m_map[idx] += str + "\n";
            } else {
                m_map[idx] += str;
            }
        }
    }

    std::string read() {
        std::string s;
        for (auto& i : m_map) {
            s += i.second;
        }
        return s;
    }
private:
    std::map<size_t, std::string> m_map;
    std::mutex m_mtx{};
};
