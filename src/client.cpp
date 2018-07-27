#include "client.hpp"

#include <iostream>

#define BUFSIZE 4096

void Client::connect()
{
	BOOL   is_success = FALSE;

	char* namedpipepath = std::getenv("NAMED_PIPE_PATH");
	if (!namedpipepath) {
		namedpipepath = "\\\\.\\pipe\\mynamedpipe123";
	}

	while (true)
	{
		m_pipe = CreateFile(
			namedpipepath,
			GENERIC_READ |
			GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			0,
			NULL);

		if (m_pipe != INVALID_HANDLE_VALUE)
			break;

		if (GetLastError() != ERROR_PIPE_BUSY)
		{
			std::cout << "Could not open pipe. GLE= " <<  GetLastError() << std::endl;
		}

		// All pipe instances are busy, so wait for 20 seconds. 

		if (!WaitNamedPipe(namedpipepath, 20000))
		{
			std::cout << "Could not open pipe: 20 second wait timed out." << std::endl;
		}
	}

	DWORD mode = PIPE_READMODE_MESSAGE;
	is_success = SetNamedPipeHandleState(
		m_pipe,
		&mode,
		NULL,
		NULL);
	if (!is_success)
	{
		std::cout << "SetNamedPipeHandleState failed. GLE= " << GetLastError() << std::endl;
	}
}
void Client::close() const
{
	CloseHandle(m_pipe);
}

std::future<std::string> Client::async_call(const std::string& order_json)
{
	return std::async(std::launch::async, &Client::call, this, order_json);
	//TODO check this c++17 .then([](string filename){ do_operation(filename); ); if it is wanted
}

std::string Client::sync_call(const std::string& order_json)
{
	return call(order_json);
}

std::string Client::call(const std::string& order_json) const
{
	DWORD  bytes_to_ead, bytes_written;
	TCHAR  buffer[BUFSIZE];
	const DWORD bytes_to_write = (DWORD)order_json.size() + 1;

	BOOL is_success = WriteFile(
		m_pipe,
		order_json.c_str(),
		bytes_to_write,
		&bytes_written,
		NULL);

	if (!is_success)
	{
		std::cout << "WriteFile to pipe failed. GLE= " << GetLastError() << std::endl;
	}

	do
	{
		is_success = ReadFile(
			m_pipe,
			buffer,
			BUFSIZE * sizeof(TCHAR),
			&bytes_to_ead,
			NULL);

		if (!is_success && GetLastError() != ERROR_MORE_DATA) {
			break;
		}
			

	} while (!is_success);  // repeat loop if ERROR_MORE_DATA

	if (!is_success)
	{
		std::cout << "ReadFile from pipe failed. GLE= " << GetLastError() << std::endl;
	}
	return std::string(buffer);
}

