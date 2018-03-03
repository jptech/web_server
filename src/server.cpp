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

        // create a socket file descriptor
        m_socket = socket(AF_INET, SOCK_STREAM, 0);

        // error check it
        if(m_socket < 0)
        {
            throw SocketCreateFailure(strerror(errno));
        }

        // populate the struct with correct port & accepting from any address
        s_addr.sin_family = AF_INET;
        s_addr.sin_port = htons(m_port_num);
        s_addr.sin_addr.s_addr = htonl(INADDR_ANY);

        // bind to the newly created socket
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

        if(m_single_user) std::cout << "Single user mode" << std::endl;

        while(1)
        {
            struct sockaddr_in c_addr;
            socklen_t c_len;
            int c_socket;
            int pid;

            listen(m_socket, 32);

            c_len = sizeof(c_addr);
            c_socket = accept(m_socket, (struct sockaddr *) &c_addr, &c_len);

            if(m_single_user == false)
            {
                pid = fork();
            }

            if(pid == 0 || m_single_user)
            {
                // we don't need the listen socket here
                if(m_single_user == false) close(m_socket);

                char *buf = new char[Server::BUFSIZE+1];

                if(c_socket < 0)
                {
                    close(c_socket);
                    std::cerr << "Error with client socket";
                    if(m_single_user == false) exit(0);
                    else continue;
                }

                #ifdef DEBUG
                std::cout << "Connection from " << inet_ntoa(c_addr.sin_addr);
                std::cout << " on port " << ntohs(c_addr.sin_port) << std::endl;
                #endif

                // Receive Request
                int ret = read(c_socket, buf, Server::BUFSIZE);

                if(ret <= 0)
                {
                    // error handling
                    std::cerr << "Read returned " << ret << std::endl;
                    close(c_socket);
                    delete buf;
                    // single user should just skip the connection; multi-processing can just kill the process
                    if(m_single_user == false) exit(0);
                    else continue;
                }

                // Send Response
                std::string strbuf = std::string(buf);
                HttpParse *parser = new HttpParse(strbuf, m_web_dir);
                HttpResponse response = parser->parse();

                try
                {
                    // have the response write to the selected socket fd
                    response.writeSocket(c_socket);
                }
                catch(ExceptionBase e)
                {
                    // error handling
                    close(c_socket);
                    delete parser;
                    delete buf;
                    std::cout << "Writing to client socket failed. Upstream msg: " << e.what();
                    // single user should just skip the connection; multi-processing can just kill the process
                    if(m_single_user == false) exit(0);
                    else continue;
                }

                // clean up memory & file descriptors
                close(c_socket);
                delete parser;
                delete buf;
                if(m_single_user == false) exit(0);
            }
            else
            {
                close(c_socket);
            }
        }
    }

    Server::~Server()
    {
        // make sure to close the listening socket 
        close(m_socket);
    }
}
