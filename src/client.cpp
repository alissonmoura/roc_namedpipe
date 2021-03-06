#include "client.hpp"

#include <array>
#include <iostream>
#include <thread>

bool Client::connect()
{
    const std::string namedpipepath = util::get_namedpipe_path();

    m_pipe = CreateFile(namedpipepath.c_str(), GENERIC_READ | GENERIC_WRITE, 0,
                        nullptr, OPEN_EXISTING, 0, nullptr);

    if (m_pipe != INVALID_HANDLE_VALUE)
    {
        return true;
    }
    else
    {
        std::cerr << "ERROR: Could not open pipe. GLE= " << GetLastError()
                  << std::endl;
        return false;
    }
}

void Client::close() const { CloseHandle(m_pipe); }

std::future<std::string> Client::async_call(const std::string & order_json)
{
    return std::async(std::launch::async, &Client::call, this,
                      order_json); // move semantics implicitly
}

std::string Client::sync_call(const std::string & order_json)
{
    return call(order_json); // move semantics implicitly
}

std::string Client::call(const std::string & order_json) const
{
    if (!write_to_pipe(order_json))
    {
        std::cerr << "ERROR: Something went wrong during sending the request. "
                     "Therefore, it will not be posksible to receive the "
                     "response from the server."
                  << std::endl;
        return std::string{};
    }

    auto response = read_from_pipe();
    if (response.empty())
    {
        std::cerr
            << "ERROR: Something went wrong during receiving the response. "
            << std::endl;
    }
    return std::string(response.data()); // move semantics implicitly
}

bool Client::write_to_pipe(const std::string & request) const
{
    DWORD bytes_written;
    const DWORD bytes_to_write = (DWORD)request.size() + 1;

    if (!WriteFile(m_pipe, request.c_str(), bytes_to_write, &bytes_written,
                   nullptr))
    {
        std::cerr << "ERROR: WriteFile to pipe failed. GLE= " << GetLastError()
                  << std::endl;
        return false;
    }
    else if (bytes_written != bytes_to_write)
    {
        std::cerr << "ERROR: The amount of data written in the pipe is not "
                     "what is supposed to be."
                  << std::endl;
        return false;
    }
    return true;
}

std::array<char, util::BUFSIZE> Client::read_from_pipe() const
{
    std::array<char, util::BUFSIZE> response;
    DWORD bytes_to_read;

    if (!ReadFile(m_pipe, &response, util::BUFSIZE * sizeof(TCHAR),
                  &bytes_to_read, nullptr))
    {
        std::cerr << "ERROR: ReadFile from pipe failed. GLE= " << GetLastError()
                  << std::endl;
        return std::array<char, util::BUFSIZE>{}; // move semantics implicitly
    }
    return response; // move semantics implicitly
}
