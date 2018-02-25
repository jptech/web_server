#pragma once
#include "exception_base.hpp"

namespace wwwserver
{
    /**
     * @class SocketCreateFailure
     * @brief A socket file descriptor could not be created.
     */
    NEW_EX(SocketCreateFailure)

    /**
     * @class SocketBindFailure
     * @brief The server could not bind to the socket.
     */
    NEW_EX(SocketBindFailure)

    /**
     * @class ClientSocketFailure
     * @brief I/O failure on the socket from/to the client
     */
     NEW_EX(ClientSocketFailure)
}
