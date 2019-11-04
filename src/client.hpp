#pragma once

#include "util.hpp"

#include <windows.h>

#include <future>
#include <string>

class Client
{
  public:
    void connect();
    void close() const;
    std::future<std::string> async_call(const std::string & order_json);
    std::string sync_call(const std::string & order_json);

  private:
    HANDLE m_pipe = nullptr;

    std::string call(const std::string & order_json) const;
    bool write_to_pipe(const std::string & request) const;
    std::array<char, util::BUFSIZE> read_from_pipe() const;
};