#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include "mime.hpp"
#include "path.hpp"

namespace wwwserver
{
    enum class HttpRequestType
    {
        GET,
        HEAD,
        POST,
        PUT,
        UNKNOWN
    };

    HttpRequestType strToRequestType(std::string &cmd);

    class HttpResponse
    {
    public:
        void generateHeader(int code);
        void setContentType(MimeType mime);
        void loadFile(Path &file);
        void loadString(std::string &content);
        void clear();
        std::string str();
    private:
        std::stringstream m_response;
        MimeType m_file_mime;
    };

    class HttpParse
    {
    public:
        HttpParse(std::string request, std::string www_dir);
        HttpRequestType getRequestType();
        HttpResponse parse();
    private:
        void parseGet(HttpResponse &response);
        void parseError(HttpResponse &response);

        std::string m_request;
        std::string m_web_dir;
        HttpRequestType m_req_type;
    };

}
