#pragma once

#include "connection.hpp"

class Server {
public:
	Server(AbstractConnection * const connection);

    void run() const;

    void stop();

private:
	AbstractConnection * const m_connection;
};


