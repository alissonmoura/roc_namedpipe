#pragma once

#include "connection.hpp"
#include "handler.hpp"

#include <windows.h> 


class NamedPipeConnection : public AbstractConnection
{
public:
	NamedPipeConnection(AbstractHandler* const handler);

	void connect() override;
private:
	AbstractHandler* const m_handler;

	void start_named_pipe_worker();

	void instance_thread(HANDLE pipe) const;
};