#include "server.hpp"
#include "connection.hpp"

#include <iostream>
#include <chrono>
#include <thread>


Server::Server(AbstractConnection * const connection) : m_connection(connection)
{
}

void Server::run() const
{
	m_connection->connect();
}


void Server::stop()
{
	//TODO
}
