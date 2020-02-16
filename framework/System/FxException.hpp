#pragma once
#ifndef F_EX_DEFINED
#define F_EX_DEFINED
#include <stdexcept>
#include "StringToken.hpp"

#define Exceptions(x,y) FxException(x,y,__FUNCTION__,__LINE__,__FILE__)
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define E_EXCEPTION 0
#define E_ERROR 1
#define E_WARNING 2
#define E_NOTICE 3

class FxException : public std::exception
{
public:
    FxException(int code, const std::string& msg, std::string callfunc = __FUNCTION__, int callline = __LINE__, std::string filename = "")
    {
        switch(code)
        {
            case E_EXCEPTION:
            std::cout << red(bold("Exception: ")) << yellow(bold(msg)) << " in " << filename << " " << yellow(bold(callfunc)) << yellow(bold("()")) << " on line " << bold(callline) << std::endl;
            break;
            
            case E_ERROR:
            std::cout << red(bold("Error: ")) << yellow(bold(msg)) << " in " << filename << " " << yellow(bold(callfunc)) << yellow(bold("()")) << " on line " << bold(callline) << std::endl;
            break;

            case E_WARNING:
            std::cout << yellow(bold("Warning: ")) << magenta(bold(msg)) << " in " << filename << " " << yellow(bold(callfunc)) << yellow(bold("()")) << " on line " << bold(callline) << std::endl;
            break;

            case E_NOTICE:
            std::cout << green(bold("Notice: ")) << bold(msg) << " in " << filename << " " << bold(callfunc) << bold("()") << " on line " << bold(callline) << std::endl;
            break;

            default:
            break;
        }
        if(code < E_WARNING)
            throw std::invalid_argument(msg);
    }
};

#endif