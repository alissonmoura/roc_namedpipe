#include "server.hpp"
#include "connection.hpp"


Server::Server(AbstractConnection & connection) : m_connection(connection)
{
}

void Server::run() const { m_connection.connect(); }