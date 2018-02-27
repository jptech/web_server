#include "server.hpp"
#include "http.hpp"
#include "exceptions.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>

#include <iostream>

namespace wwwserver
{
    void Server::setup()
    {
        struct sockaddr_in s_addr;

        m_socket = socket(AF_INET, SOCK_STREAM, 0);

        if(m_socket < 0)
        {
            throw SocketCreateFailure(strerror(errno));
        }

        s_addr.sin_family = AF_INET;
        s_addr.sin_port = htons(m_port_num);
        s_addr.sin_addr.s_addr = htonl(INADDR_ANY);

        if(bind(m_socket, (struct sockaddr *) &s_addr, sizeof(s_addr)) < 0)
        {
            throw SocketBindFailure(strerror(errno));
        }

        m_setup = true;
    }

    void Server::loop()
    {
        if(!m_setup)
        {
            throw SocketCreateFailure("Socket was not initialized");
        }

        // bad parent -- we don't care about our children's death
        // this prevents zombie processes by ignoring SIGCHILD
        signal(SIGCHLD, SIG_IGN);

        while(1)
        {
            struct sockaddr_in c_addr;
            socklen_t c_len;
            int c_socket;
            char *buf = new char[Server::BUFSIZE+1];

            listen(m_socket, 5);

            c_len = sizeof(c_addr);
            c_socket = accept(m_socket, (struct sockaddr *) &c_addr, &c_len);

            int pid = fork();

            if(pid == 0)
            {
                // we don't need the listen socket here
                close(m_socket);

                if(c_socket < 0)
                {
                    close(c_socket);
                    throw ClientSocketFailure("Error with client socket");
                }

                std::cout << "Connection from " << inet_ntoa(c_addr.sin_addr);
                std::cout << " on port " << ntohs(c_addr.sin_port) << std::endl;

                // Receive Request
                int ret = read(c_socket, buf, Server::BUFSIZE);

                if(ret <= 0)
                {
                    // error handling
                    std::cerr << "Read returned " << ret << std::endl;
                    close(c_socket);
                    throw ClientSocketFailure("Reading from client socket failed.");
                }

                // Send Response
                std::string strbuf = std::string(buf);
                HttpParse *parser = new HttpParse(strbuf, m_web_dir);
                HttpResponse response = parser->parse();

                try
                {
                    response.writeSocket(c_socket);
                }
                catch(ExceptionBase e)
                {
                    close(c_socket);

                    std::stringstream ss;
                    ss << "Writing to client socket failed. Upstream msg: " << e.what();
                    throw ClientSocketFailure(ss.str());
                }

                close(c_socket);
                delete parser;
                delete buf;
                exit(0);
            }

            close(c_socket);
        }
    }

    Server::~Server()
    {
        close(m_socket);
    }
}
