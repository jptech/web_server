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
    // print usage information
    std::cerr << "Usage: " << prgm_name << " [options]" << std::endl;
    std::cerr << std::endl << "Options:" << std::endl;
    std::cerr << "\t-h (help)" << std::endl;
    std::cerr << "\t-p [port #]" << std::endl;
    std::cerr << "\t-w [web root]" << std::endl;
    std::cerr << "\t-s [enable single user/disable mutli-process]" << std::endl;
    std::cerr << std::endl;

    exit(1);
}

int main(int argc, char **argv)
{
    std::stringstream ss;
    std::string arg;
    int port = 8000;
    bool single_user = false;

    // print our generic server name
    std::cout << "wwwserver/0.1" << std::endl;

    // pick the default web root directory
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
        else if(arg == "-s")
        {
            single_user = true;
        }
        else
        {
            std::cerr << "Could not parse argument: " << arg << std::endl;
            usage(argv[0]);
        }
    }

    // Let the user know the set up parameters
    std::cout << "Setting up server with web root of " << web_dir.str() << " on port " << port << std::endl;

    // create the server
    wwwserver::Server s(port, web_dir.str(), single_user);

    // attempt to set up the socket
    try
    {
        s.setup();
    }
    catch(wwwserver::ExceptionBase e)
    {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    // listen and service requests in the main loop of the server
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
