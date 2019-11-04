#include "server.hpp"
#include "connection.hpp"


Server::Server(AbstractConnection & connection) : m_connection(connection)
{
}

bool Server::run() const { return m_connection.connect(); }