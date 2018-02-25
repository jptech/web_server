#include "http.hpp"
#include "path.hpp"

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

    void HttpResponse::loadFile(Path &file)
    {
        std::ifstream ifile;
        std::string path = file.str();
        // todo: error checking
        ifile.open(path.c_str());

        std::cout << "File Extension: " << file.extension() << std::endl;

        // mime type
        m_response << "Content-Type: " << get_type_string(get_mime_type(file.extension())) << std::endl;

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

    void HttpParse::parseGet(HttpResponse &response)
    {
        int response_code = 200;
        std::stringstream ss(m_request);
        std::string cmd, req_file, http_ver;

        ss >> cmd >> req_file >> http_ver;

        Path file = Path(m_web_dir + req_file);

        std::cout << "Opening " << file.str() << std::endl;

        if(! file.exists())
        {
            response_code = 404;
            std::cout << "File not found" << std::endl;
        }

        if(file.isDir())
        {
            Path idx_file = file + "/index.html";
            std::cout << "Looking for " << idx_file.str() << std::endl;

            if(idx_file.exists())
            {
                file.setPath(idx_file.str());
            }
            else
            {
                response_code = 404; // not found for now
                // todo: directory listing
            }
        }

        response.generateHeader(response_code);

        if(response_code == 200)
        {
            std::cout << "File found" << std::endl;
            response.loadFile(file);
        }
    }

    void HttpParse::parseError(HttpResponse &response)
    {
        response.generateHeader(500); // internal error
    }
}
