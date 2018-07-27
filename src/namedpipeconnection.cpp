#include "namedpipeconnection.hpp"

#include <cstdlib>
#include <thread>
#include <iostream>
#include <string>


#define BUFSIZE 4096

NamedPipeConnection::NamedPipeConnection(AbstractHandler* const handler) : m_handler(handler)
{
}

void NamedPipeConnection::connect()
{
	start_named_pipe_worker();
}

void NamedPipeConnection::start_named_pipe_worker()
{
	BOOL   is_connected = FALSE;
	HANDLE pipe = INVALID_HANDLE_VALUE, hThread = NULL;
	
	const char* namedpipepath = std::getenv("NAMED_PIPE_PATH");
	if (!namedpipepath) {
		namedpipepath = "\\\\.\\pipe\\mynamedpipe123";
	}

	for (;;)
	{
		std::cout << "Pipe Server: Main thread awaiting client connection on " << std::string(namedpipepath) << std::endl;
		pipe = CreateNamedPipe(
			namedpipepath,             // pipe name 
			PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,       // read/write access 
			PIPE_TYPE_MESSAGE |       // message type pipe 
			PIPE_READMODE_MESSAGE |   // message-read mode 
			PIPE_WAIT,                // blocking mode 
			PIPE_UNLIMITED_INSTANCES, // max. instances  
			BUFSIZE,                  // output buffer size 
			BUFSIZE,                  // input buffer size 
			0,                        // client time-out 
			NULL);                    // default security attribute

		if (pipe == INVALID_HANDLE_VALUE)
		{
			std::cout << "CreateNamedPipe failed, GLE " << GetLastError() << std::endl;
		}

		is_connected = ConnectNamedPipe(pipe, NULL) ?
			true : (GetLastError() == ERROR_PIPE_CONNECTED);

		if (is_connected)
		{
			std::cout << "Client connected, creating a processing thread." << std::endl;
			std::thread thread(&NamedPipeConnection::instance_thread, this, pipe);
			thread.detach();
		}
		else {
			// The client could not connect, so close the pipe. 
			CloseHandle(pipe);
		}
		//TODO check a way to close named pipe for clients that were killed with sigkill for instance
	}
}

void NamedPipeConnection::instance_thread(HANDLE pipe) const
{
	HANDLE heap_handle = GetProcessHeap();
	TCHAR* request = (TCHAR*)HeapAlloc(heap_handle, 0, BUFSIZE * sizeof(TCHAR));
	TCHAR* reply = (TCHAR*)HeapAlloc(heap_handle, 0, BUFSIZE * sizeof(TCHAR));

	DWORD bytes_to_read = 0, bytes_to_reply = 0, bytes_witten = 0;
	BOOL success = FALSE;

	if (pipe == NULL)
	{
		//TODO log error
		if (reply != NULL) HeapFree(heap_handle, 0, reply);
		if (request != NULL) HeapFree(heap_handle, 0, request);
	}

	if (request == NULL)
	{
		//TODO log error
		if (reply != NULL) HeapFree(heap_handle, 0, reply);
	}

	if (reply == NULL)
	{
		//TODO log error
		if (request != NULL) HeapFree(heap_handle, 0, request);
	}

	while (true)
	{
		success = ReadFile(
			pipe,
			request,
			BUFSIZE * sizeof(TCHAR),
			&bytes_to_read,
			NULL);

			if (!success || bytes_to_read == 0)
			{
				if (GetLastError() == ERROR_BROKEN_PIPE)
				{
					//TODO log error "InstanceThread: client disconnected"
				}
				else
				{
					//TODO log error "InstanceThread ReadFile failed, GLE= GetLastError()
				}
				break;
			}

		auto ret = m_handler->handle(request);
		std::cout << "writing....." << std::endl;
		std::cout << ret << std::endl;

		success = WriteFile(
			pipe,
			ret.c_str(),
			(DWORD)ret.size() + 1,
			&bytes_witten,
			NULL);

		if (!success || (DWORD)ret.size() + 1 != bytes_witten)
		{
			std::cout << "InstanceThread WriteFile failed, GLE= " << GetLastError() << std::endl;
			break;
		}
	}

	FlushFileBuffers(pipe);
	DisconnectNamedPipe(pipe);
	CloseHandle(pipe);

	HeapFree(heap_handle, 0, request);
	HeapFree(heap_handle, 0, reply);

	std::cout << "InstanceThread exitting.\n" << std::endl;
}
