#include "server.hpp"
#include "http.hpp"
#include "exceptions.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <string.h>

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

        std::cout << "Socket binded to port " << ntohs(s_addr.sin_port) << std::endl;

        m_setup = true;
    }

    void Server::loop()
    {
        if(!m_setup)
        {
            throw SocketCreateFailure("Socket was not initialized");
        }

        while(1)
        {
            struct sockaddr_in c_addr;
            socklen_t c_len;
            int c_socket;
            char *buf = new char[Server::BUFSIZE+1];

            std::cout << "Listening on socket " << m_socket << std::endl;
            listen(m_socket, 5);

            c_len = sizeof(c_addr);
            c_socket = accept(m_socket, (struct sockaddr *) &c_addr, &c_len);

            if(c_socket < 0)
            {
                std::cerr << "c_socket error " << c_socket << std::endl;
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

            std::cout << "Read (ret: " << ret << "): " << std::endl;
            std::cout << buf << std::endl;

            // Send Response
            //HttpParse client_parser(std::string(buf));
            std::string strbuf = std::string(buf);
            HttpParse *parser = new HttpParse(strbuf, m_web_dir);
            HttpResponse response = parser->parse();

            std::cout << std::endl << "Response: " << std::endl << std::endl;
            std::cout << response.str().c_str() << std::endl;

            ret = write(c_socket, response.str().c_str(), response.str().size()+1);
            if(ret <= 0)
            {
                // error handling
                std::cerr << "Write returned " << ret << std::endl;
                close(c_socket);
                throw ClientSocketFailure("Writing to client socket failed.");
            }

            delete parser;
            delete buf;
            close(c_socket);
        }
    }

    Server::~Server()
    {
        close(m_socket);
    }
}
