#pragma once

#include <windows.h> 

#include <future>
#include <string>


class Client
{
public:
	void connect();
	void close() const;
	std::future<std::string> async_call(const std::string& order_json);
	std::string sync_call(const std::string& order_json);

private:
	HANDLE m_pipe = nullptr;

	std::string call(const std::string& order_json) const;
};