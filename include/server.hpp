#pragma once

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <string>
#include <thread>

namespace wwwserver
{
    class Server
    {
    public:


        /**
         * Constructor
         * @param port The server will be hosted at this port.
         * @param web_dir The directory to be hosted by the server.
         */
        Server(int port, std::string web_dir) : m_web_dir(web_dir), m_setup(false), m_port_num(port) {}

        /**
         * setup
         * @brief Creates and binds to a POSIX socket.
         */
        void setup();

        /**
         * loop
         * @brief Listens for and then services connections.
         */
        void loop();

        ~Server();

        /* 8KB standard buffer size */
        static const size_t BUFSIZE = 8096;

    private:
        std::string m_web_dir;
        bool m_setup;
        int m_port_num;
        int m_socket;

    };
}
