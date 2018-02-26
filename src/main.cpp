#include <iostream>
#include <sstream>

#include <unistd.h>

#include "exception_base.hpp"
#include "exceptions.hpp"
#include "server.hpp"
#include "path.hpp"

using wwwserver::Path;

void usage(char *prgm_name)
{
    std::cerr << "Usage: " << prgm_name << " [options]" << std::endl;
    std::cerr << std::endl << "Options:" << std::endl;
    std::cerr << "\t-h (help)" << std::endl;
    std::cerr << "\t-p [port #]" << std::endl;
    std::cerr << "\t-w [web root]" << std::endl;
    std::cerr << std::endl;

    exit(1);
}

int main(int argc, char **argv)
{
    std::stringstream ss;
    std::string arg;
    int port = 8000;

    std::cout << "wwwserver/0.1" << std::endl;

    Path my_dir;
    Path web_dir(my_dir.str() + "/www");

    for(int i = 1; i < argc; i++)
    {
        ss << argv[i] << " ";
    }

    while(ss >> arg)
    {
        if(arg == "-h")
        {
            usage(argv[0]);
        }
        else if(arg == "-p")
        {
            if(!(ss >> port))
            {
                usage(argv[0]);
            }
        }
        else if(arg == "-w")
        {
            if(!(ss >> arg))
            {
                usage(argv[0]);
            }
            web_dir.setPath(arg);
        }
        else
        {
            std::cerr << "Could not parse argument: " << arg << std::endl;
            usage(argv[0]);
        }
    }

    std::cout << "Setting up server with web root of " << web_dir.str() << " on port " << port << std::endl;

    wwwserver::Server s(port, web_dir.str());

    try
    {
        s.setup();
    }
    catch(wwwserver::ExceptionBase e)
    {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    try
    {
        s.loop();
    }
    catch(wwwserver::ExceptionBase e)
    {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    return 0;
}
