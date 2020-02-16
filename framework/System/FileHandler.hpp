#pragma once
#ifndef F_H_DEFINED
#define F_H_DEFINED
#include <fstream>
#include <string>

class FileHandler{
    public:
        std::string read(std::string fname);
        bool write(std::string fname, std::string value);
        bool write_relay(std::string fname, std::string value);

        FileHandler()
        {

        }
};


std::string FileHandler::read(std::string fname)
{
    std::ifstream in(fname);
    std::string s;

    if (in.is_open()) {
        in.seekg(0, std::ios::end);
        int size = in.tellg();
        s.resize(size);
        in.seekg(0, std::ios::beg);
        in.read(&s[0], size);
    } else {
        throw std::invalid_argument("INVAILD_FILENAME");
    }

    in.close();
    return s;
}

bool FileHandler::write(std::string fname, std::string value)
{
    std::ofstream out(fname);
    
    if (out.is_open()) {
        out << value;
    } else {
        throw std::invalid_argument("INVAILD_FILENAME");
    }

    return true;
}

bool FileHandler::write_relay(std::string fname, std::string value)
{
    std::ofstream out(fname, std::ios::app);
    
    if (out.is_open()) {
        out << value;
    } else {
        throw std::invalid_argument("INVAILD_FILENAME");
    }

    return true;
}
#endif