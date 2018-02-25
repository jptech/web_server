#include <iostream>
#include <sstream>

#include <unistd.h>

#include "server.hpp"

int main(int argc, char **argv)
{
    int port = 8000;
    char cur_dir[255];
    getcwd(cur_dir, sizeof(cur_dir));
    std::string web_dir = std::string(cur_dir) + "/www";

    std::cout << "Web Dir: " << web_dir << std::endl;

    for(int i = 0; i < argc; i++)
    {
        std::stringstream ss(argv[i]);
        std::string str;
        ss >> str;
        std::cout << "Arg " << i << ": " << str << std::endl;
    }

    if(argc > 1)
    {
        port = atoi(argv[1]);
    }

    wwwserver::Server s(port, web_dir);

    std::cout << "Setting up socket" << std::endl;
    try
    {
        s.setup();
    }
    catch(std::exception e)
    {
        std::cerr << e.what() << std::endl;
    }

    std::cout << "Listening on " << port << std::endl;
    try
    {
        s.loop();
    }
    catch(std::exception e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
