#pragma once

#include "connection.hpp"

/**
 * @brief The server for the communication. It can communicate with many
 * clients. All the answers are in JSON formata including the errors.
 *
 */
class Server
{
  public:
    /**
     * @brief Construct a new Server object
     *
     * @param connection It needs the Interface connection to be constructed.
     * The interface keeps the user (server_test.cpp) from the implemmentation
     * so it is easier to change it in the future if needed.
     */
    Server(AbstractConnection & connection);

    /**
     * @brief run the server
     *
     * @return bool It tells if the server is running fine or not.
     */
    bool run() const;

  private:
    AbstractConnection & m_connection;
};
