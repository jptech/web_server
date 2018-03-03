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
        Server(int port, std::string web_dir, bool single_user = false) :
            m_web_dir(web_dir), m_setup(false), m_port_num(port), m_single_user(single_user) {}

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

        /**
         * Destructor
         */
        ~Server();

        /* 8KB standard buffer size */
        static const size_t BUFSIZE = 8096;

    private:
        /* location of the top level web directory */
        std::string m_web_dir;

        /* value to designate if the server has been set up correctly */
        bool m_setup;

        /* the port number to listen for new connections */
        int m_port_num;

        /* the socket file descriptor for listenting for new connections */
        int m_socket;

        /* toggles the single user mode when true; uses multi-processing when false */
        bool m_single_user;

    };
}
