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
     * @class
     * @brief
     */
     NEW_EX(ClientSocketFailure)
}
