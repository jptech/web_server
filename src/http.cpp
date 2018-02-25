#include "http.hpp"

namespace wwwserver
{
    HttpRequestType strToRequestType(std::string &cmd)
    {
        HttpRequestType ret;

        if(cmd == "GET")
        {
            ret = HttpRequestType::GET;
        }
        else if(cmd == "HEAD")
        {
            ret = HttpRequestType::HEAD;
        }
        else if(cmd == "POST")
        {
            ret = HttpRequestType::POST;
        }
        else if(cmd == "PUT")
        {
            ret = HttpRequestType::PUT;
        }
        else
        {
            ret = HttpRequestType::UNKNOWN;
        }

        return ret;
    }

    void HttpResponse::clear()
    {
        m_response.clear();
    }

    void HttpResponse::generateHeader(int code)
    {
        m_response << "HTTP/1.0 " << code << std::endl;
    }

    void HttpResponse::loadFile(std::string &fname)
    {
        std::ifstream ifile;
        // todo: error checking
        ifile.open(fname.c_str());
        loadFile(ifile);
    }

    void HttpResponse::loadFile(std::ifstream &ifile)
    {
        // mime type
        m_response << "Content-Type: " << get_type_string(m_file_mime) << std::endl;

        // todo: content length

        // file content
        // todo? -- NOT EFFICIENT
        m_response << std::endl;
        m_response << ifile.rdbuf();
    }

    void HttpResponse::setContentType(MimeType mime)
    {
        m_file_mime = mime;
    }

    void HttpResponse::loadString(std::string &content)
    {
        m_response << "Content-Type: text/html" << std::endl;

        m_response << std::endl;
        m_response << content;
    }

    std::string HttpResponse::str()
    {
        return m_response.str();
    }

    HttpParse::HttpParse(std::string request, std::string web_dir)
    {
        std::stringstream ss;
        std::string req_cmd;
        m_request = request;

        ss << request;
        ss >> req_cmd;

        m_req_type = strToRequestType(req_cmd);
        m_web_dir = web_dir;
    }

    HttpRequestType HttpParse::getRequestType()
    {
        return m_req_type;
    }

    HttpResponse HttpParse::parse()
    {
        HttpResponse response;

        switch(m_req_type)
        {
            case HttpRequestType::GET:
                parseGet(response);
                break;
            case HttpRequestType::HEAD:
            case HttpRequestType::PUT:
            case HttpRequestType::POST:
            default:
                parseError(response);
                std::cerr << "Request type not yet supported" << std::endl;
                break;
        }

        return response;
    }

    void HttpParse::cleanFilePath(std::string &fname)
    {
        // todo
        fname = m_web_dir + fname;
    }

    void HttpParse::parseGet(HttpResponse &response)
    {
        int response_code = 200;
        std::stringstream ss(m_request);
        std::string cmd, file, http_ver;

        ss >> cmd >> file >> http_ver;

        cleanFilePath(file);
        std::cout << "Opening " << file << std::endl;

        std::ifstream ifile;
        ifile.open(file.c_str());

        if(! ifile.good())
        {
            response_code = 404;
            std::cout << "File not found" << std::endl;
        }

        if(response_code == 200) response.setContentType(MimeType::HTML);
        response.generateHeader(response_code);

        if(response_code == 200)
        {
            std::cout << "File found" << std::endl;
            response.loadFile(ifile);
        }

        ifile.close();
    }

    void HttpParse::parseError(HttpResponse &response)
    {
        int response_code = 400; // bad request

        response.generateHeader(400);
    }
}
