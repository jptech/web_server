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

    /**
     * strToRequestType
     * @brief Determines what type of HTTP request is being sent
     * @see HttpRequestType
     * @return The enum value corresponding the the approriate request type
     */
    HttpRequestType strToRequestType(std::string &cmd);

    class HttpResponse
    {
    public:

        /**
         * Constructor
         * @param web_dir The top level public web directory
         */
        HttpResponse(std::string web_dir) : m_web_dir(web_dir) {}

        /**
         * generateHeader
         * @brief Generates the pre-content HTTP header with specified response code
         * @param code The HTTP/1.0 response code
         */
        void generateHeader(int code);

        /**
         * loadFile
         * @brief Loads a file as the content of the HTTP response
         * @param file A path object to the desired file
         */
        void loadFile(Path &file);

        /**
         * loadString
         * @brief Loads a string as the content of the HTTP response
         * @param content A string of the desired response content
         */
        void loadString(std::string &content);

        /**
         * loadDirListing
         * @brief Loads a directory listing as the content of the HTTP response
         * @param dir The directory to list
         */
        void loadDirListing(Path &dir);

        /**
         * clear
         * @brief Clears the state of the HttpResponse object
         */
        void clear();

        /**
         * str
         * @brief Generates the HTTP response as a string
         * @return HTTP response as a string object
         */
        std::string str();

    private:

        /* A string stream used to build the HTTP response */
        std::stringstream m_response;

        /* The web server top level directory */
        std::string m_web_dir;
     };

    class HttpParse
    {
    public:

        /**
         * Constructor
         * @param request The incoming request from the HTTP client
         * @param www_dir The top level web directory
         */
        HttpParse(std::string request, std::string www_dir);

        /**
         * getRequestType
         * @see HttpRequestType
         * @return Type of incoming request
         */
        HttpRequestType getRequestType();

        /**
         * parse
         * @brief Parses the incoming request
         * @return The server's respose to the client's request
         */
        HttpResponse parse();
    private:

        /**
         * parseGet
         * @brief Parses and services the HTTP GET command
         * @param response The outgoing HTTP response
         */
        void parseGet(HttpResponse &response);

        /**
         * parseError
         * @brief The default error case handling
         * @param response The outgoing HTTP response
         */
        void parseError(HttpResponse &response);

        /* the HTTP request to the server */
        std::string m_request;

        /* the top level web directory */
        std::string m_web_dir;

        /* the type of request received */
        HttpRequestType m_req_type;
    };

}
