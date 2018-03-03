#include "http.hpp"
#include "path.hpp"
#include "exceptions.hpp"
#include <cstring>
#include <map>

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
        m_load_file = false;
    }

    void HttpResponse::generateHeader(int code)
    {
        m_response << "HTTP/1.0 " << code << std::endl;
        m_response << "Server: wwwserver/0.1" << std::endl;
    }

    void HttpResponse::loadFile(Path &file)
    {
        // mime type
        m_response << "Content-Type: " << get_type_string(get_mime_type(file.extension())) << std::endl;

        // content length
        m_response << "Content-Length: " << file.filesize() << std::endl << std::endl;

        m_load_file = true;
        m_file.setPath(file.str());
    }

    void HttpResponse::loadCgi(Path &file)
    {
        /* the old stdin, used by the parent to temporarily hold stdin while it is being used */
        int oldin;
        /* the pipe file descriptors, 0 is the read end */
        int pipefd[2];
        /* the process id: 0 if this is the child, and the pid of the child if the parent */
        int pid;
        char *alist = NULL;
        char *pargv[3];
        std::string line;
        
        m_response << "Content-Type: text/html" << std::endl << std::endl;
        pargv[0] = strdup(file.str().c_str());
        pargv[1] = "7";
        pargv[2] = NULL;

        if(pipe(pipefd) != 0)
        {
            m_response << "CGI Error: Failed to open pipe\n";
        }
        else
        {
            m_response << "<html>" << std::endl;
            m_response << "<body><p> CGI output for " << file.str() <<"</p>" << std::endl;
            pid = fork();
            
            /* child fork */
            if(pid == 0)
            {
                //copy stdout to the end of the pipe that reads
                dup2(pipefd[1], 1);
                //close both ends of the pipe child-side so that the os can close the pipe gracefully
                close(pipefd[1]);
                close(pipefd[0]);

                //std::cout << "TEST " <<  alist[0] << std::endl;
                /* run the cgi script */
                execvp(file.str().c_str(), pargv);
                std::cerr << "Failure to launch cgi\n";
                m_response << "CGI Error: Failed to open " << file.str() 
                    << "</body></html>" << std::endl;
            }
            /* parent fork */
            else if (pid == -1)
            {
                m_response << "CGI Error: Fork failure."
                    << "</body></html>" << std::endl;
            }
            else
            {
                /* stash stdin during cgi script */
                oldin = dup(0);
                /* handle pipes as in child, except stdin */
                dup2(pipefd[0], 0);
                close(pipefd[0]);
                close(pipefd[1]);

                //m_response << "Content-Type: text/html" << std::endl;
                m_response << "<html>" << std::endl;
                m_response << "\t<title> CGI output for " << file.str() <<"</title>" << std::endl;
                m_response << "<body>" << std::endl;

                while(getline(std::cin, line))
                {
                    m_response << line << std::endl;
                }
                m_response << "</body> </html>" << std::endl;
                //this should kill the write end of the pipe
                dup2(oldin, 0);

            }
        }
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

    void HttpResponse::writeSocket(int socket_fd)
    {
        int ret = write(socket_fd, m_response.str().c_str(), m_response.str().size());
        if(ret < 0)
        {
            std::stringstream ss;
            ss << "Socket write returned " << ret;
            throw ClientSocketFailure(ss.str());
        }

        if(m_load_file)
        {
            const int buf_size = 8192;
            char buf[buf_size]; // 8KB buffer

            int file_fd = open(m_file.str().c_str(), O_RDONLY);

            int rd = read(file_fd, buf, buf_size);
            int wr = 1;

            while(rd > 0 && wr > 0)
            {
                wr = write(socket_fd, buf, rd);
                rd = read(file_fd, buf, buf_size);
            }

            close(file_fd);
        }
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
            case HttpRequestType::POST:
                parsePost(response);
                break;
            case HttpRequestType::HEAD:
            case HttpRequestType::PUT:
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

        if(! file.exists())
        {
            response_code = 404;
            std::cout << "File not found" << std::endl;
        }

        if(file.isDir())
        {
            Path idx_file = file + "/index.html";

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
            if(send_file) 
            {
                if(file.isCgi())
                {
                    response.loadCgi(file);
                }
                else
                {
                    response.loadFile(file);
                }
            }
            else response.loadDirListing(file);
        }
        else
        {
            response.loadError(response_code);
        }
    }

    void HttpParse::parsePost(HttpResponse &response)
    {
        int response_code = 200;
        std::stringstream sin(m_request);
        std::string line;
        std::ofstream fout;

        while(getline(sin, line) && line.size() > 2);
        fout.open("formdata.txt");
        while(getline(sin, line))
        {
            fout << line << std::endl;
        }
        fout.close();

        parseGet(response);
    }

    void HttpParse::parseError(HttpResponse &response)
    {
        response.generateHeader(500); // internal error
    }

}
