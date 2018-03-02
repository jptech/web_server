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
        HttpResponse(std::string web_dir) : m_web_dir(web_dir), m_load_file(false) {}

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
         * loadCgi
         * @brief Executes a cgi script and loads the response
         * @param file A path object to the desired cgi script
         */
        void loadCgi(Path &file);

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
         * loadError
         * @brief Loads the approriate error page into the response.
         * @param code The integer HTTTP status code corresponding to the error.
         */
        void loadError(int code);

        /**
         * clear
         * @brief Clears the state of the HttpResponse object
         */
        void clear();

        /**
         * writeSocket
         * @brief Writes the response to the desired socket file descriptor
         * @param socket_fd The socket file descriptor
         */
        void writeSocket(int socket_fd);

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

        /* Path to file to send */
        Path m_file;

        /* Indicate if a file should be sent */
        bool m_load_file;
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
         * @brief Parses and services form actions via POST commands
         * 
         * @param response The outgoing HTTP response
         */
        void parsePost(HttpResponse &response);

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
