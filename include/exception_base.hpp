#pragma once
#include <string>

#define NEW_EX(name) \
class name : public wwwserver::ExceptionBase \
{ \
public: \
    name(std::string message = "") : wwwserver::ExceptionBase(#name ": " + message) {} \
};

namespace wwwserver
{
    class ExceptionBase : public std::exception
    {
    protected:
        std::string m_message;

    public:
        ExceptionBase(std::string message = "") : m_message(message) {}
        const char* what() const throw()
        {
            return m_message.c_str();
        }
    };
}
