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
        m_response << "Server: wwwserver/0.1" << std::endl;
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

    void HttpResponse::loadString(std::string &content)
    {
        m_response << "Content-Type: text/html" << std::endl;

        m_response << std::endl;
        m_response << content;
    }

    void HttpResponse::loadDirListing(Path &dir)
    {
        m_response << "Content-Type: text/html" << std::endl << std::endl;

        std::vector<Path> dirs = dir.getChildrenDirs();
        std::vector<Path> files = dir.getChildrenFiles();

        std::string web_dir_str = m_web_dir;
        std::string dir_str = dir.str().substr(web_dir_str.size());

        std::cout << "Listing for " << dir_str << std::endl;
        std::cout << dirs.size() << " subdirectories, " << files.size() << " files" << std::endl;

        // Print out the heading for the listing
        m_response << "<!DOCTYPE html>" << std::endl
                   << "<html>" << std::endl
                   << "<head>" << std::endl
                   << "\t<title>Listing for " << dir_str << "</title>" << std::endl
                   << "\t<style type='text/css'>"
                   << "\t\tbody {"<< std::endl
                   << "\t\t\tfont-family: sans-serif;" << std::endl
                   << "\t\t\tfont-size: 14px;" << std::endl
                   << "\t\t}" << std::endl
                   << "\t</style>" << std::endl
                   << "</head>" << std::endl
                   << "<body>" << std::endl
                   << "\t<h1>Directory Listing</h1>" << std::endl
                   << "\t<h2>" << dir_str << "</h2>" << std::endl
                   << "\t<ul>" << std::endl;

        if(dir_str.size() != 0)
        {
            m_response << "\t\t<li><a href='" << dir_str << "/..'>Parent Directory</a></li>" << std::endl;
        }

        // Process subdirectories first
        for(Path const &p : dirs)
        {
            std::string path_str = "/" + p.str().substr(web_dir_str.size()+1);
            std::string display_str = p.str().substr(web_dir_str.size() + dir_str.size() + 1);
            m_response << "\t\t<li><a href='" << path_str << "'>" << display_str << "</a></li>" << std::endl;
        }

        // Process all the files
        for(Path const &p : files)
        {
            std::string path_str = "/" + p.str().substr(web_dir_str.size()+1);
            std::string display_str = p.str().substr(web_dir_str.size() + dir_str.size() + 1);
            m_response << "\t\t<li><a href='" << path_str << "'>" << display_str << "</a></li>" << std::endl;
        }

        // close out the page
        m_response << "\t</ul>" << std::endl
                   << "<hr>" << std::endl
                   << "<footer><small>wwwserver/0.1</small></footer>" << std::endl
                   << "</body>" << std::endl
                   << "</html>" << std::endl;
    }

    void HttpResponse::loadError(int code)
    {
        std::string error_msg;

        switch(code)
        {
            case 404: error_msg = "Page Not Found"; break;
            default:
            case 500: error_msg = "Internal Server Error"; break;
        }

        m_response << "Content-Type: text/html" << std::endl << std::endl;

        m_response << "<!DOCTYPE html>" << std::endl
                   << "<html>" << std::endl
                   << "<head>" << std::endl
                   << "\t<title>" << error_msg << "</title>" << std::endl
                   << "\t<style type='text/css'>"
                   << "\t\tbody {"<< std::endl
                   << "\t\t\tfont-family: sans-serif;" << std::endl
                   << "\t\t\tfont-size: 14px;" << std::endl
                   << "\t\t}" << std::endl
                   << "\t</style>" << std::endl
                   << "</head>" << std::endl
                   << "<body>" << std::endl
                   << "<h1>HTTP/1.0 " << code << " " << error_msg << "</h1>" << std::endl
                   << "<hr>" << std::endl
                   << "<footer><small>wwwserver/0.1</small></footer>" << std::endl
                   << "</body>" << std::endl
                   << "</html>" << std::endl;
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
        HttpResponse response(m_web_dir);

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
        bool send_file = true;
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
                send_file = false;
            }
        }

        response.generateHeader(response_code);

        if(response_code == 200)
        {
            if(send_file) response.loadFile(file);
            else response.loadDirListing(file);
        }
        else
        {
            response.loadError(response_code);
        }
    }

    void HttpParse::parseError(HttpResponse &response)
    {
        response.generateHeader(500); // internal error
    }
}
