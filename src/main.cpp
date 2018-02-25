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

    std::cout << "Web Root: " << web_dir << std::endl;

    if(argc > 1)
    {
        port = atoi(argv[1]);
    }

    wwwserver::Server s(port, web_dir);

    try
    {
        s.setup();
    }
    catch(std::exception e)
    {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    try
    {
        s.loop();
    }
    catch(std::exception e)
    {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    return 0;
}
