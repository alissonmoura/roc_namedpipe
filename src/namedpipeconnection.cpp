#include "namedpipeconnection.hpp"
#include "util.hpp"

#include <array>
#include <iostream>
#include <string>
#include <thread>

static bool read_from_pipe(std::array<char, util::BUFSIZE> & request,
                           const HANDLE & pipe);
static bool write_to_pipe(const std::string & ret, const HANDLE & pipe);

NamedPipeConnection::NamedPipeConnection(AbstractHandler & handler)
    : m_handler{handler}
{
}

void NamedPipeConnection::connect() { start_named_pipe_worker(); }

void NamedPipeConnection::start_named_pipe_worker()
{

    std::string namedpipepath = util::get_namedpipe_path();

    while (true)
    {
        std::cout
            << "INFO: Pipe Server: Main thread awaiting client connection on "
            << namedpipepath << std::endl;
        HANDLE pipe = CreateNamedPipe(
            namedpipepath.c_str(),
            PIPE_ACCESS_DUPLEX, // read/write
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            // message type pipe | message-read mode |  blocking mode
            PIPE_UNLIMITED_INSTANCES, // max. instances
            util::BUFSIZE,            // output buffer size
            util::BUFSIZE,            // input buffer size
            0,                        // client time-out
            nullptr);                 // default security attribute

        if (pipe == INVALID_HANDLE_VALUE)
        {
            std::cerr << "ERROR: CreateNamedPipe failed, GLE " << GetLastError()
                      << std::endl;
        }

        if (ConnectNamedPipe(pipe, nullptr))
        {
            std::cout << "INFO: Client connected, creating a processing thread."
                      << std::endl;
            std::thread thread{&NamedPipeConnection::instance_thread, this,
                               pipe};
            thread.detach();
        }
        else
        {
            std::cout
                << "WARNING: The client could not connect, so close the pipe.."
                << std::endl;
            CloseHandle(pipe);
        }
    }
}

void NamedPipeConnection::instance_thread(const HANDLE pipe) const
{
    while (true)
    {
        std::array<char, util::BUFSIZE> request{};
        if (!read_from_pipe(request, pipe))
            break;

        std::string response = m_handler.handle(request.data());

        if (!write_to_pipe(response, pipe))
            break;
    }

    FlushFileBuffers(pipe);
    DisconnectNamedPipe(pipe);
    CloseHandle(pipe);

    std::cout << "INFO: InstanceThread exitting.\n" << std::endl;
}

static bool read_from_pipe(std::array<char, util::BUFSIZE> & request,
                           const HANDLE & pipe)
{
    unsigned long bytes_to_read = 0;

    if (!ReadFile(pipe, &request, util::BUFSIZE * sizeof(TCHAR), &bytes_to_read,
                  nullptr))
    {
        if (GetLastError() == ERROR_BROKEN_PIPE)
        {
            std::cout << "WARNING: The error ERROR_BROKEN_PIPE happened while "
                         "reading."
                      << "Probably the user has disconnected." << std::endl;
        }
        else if (bytes_to_read == 0)
        {
            std::cout << "WARNING: 0 bytes was read from client!" << std::endl;
        }
        else
        {
            std::cerr << "ERROR: InstanceThread ReadFile failed, GLE= "
                      << GetLastError() << std::endl;
        }
        return false;
    }
    return true;
}

static bool write_to_pipe(const std::string & response, const HANDLE & pipe)
{
    DWORD bytes_witten = 0;

    if (!WriteFile(pipe, response.c_str(), (DWORD)response.size() + 1,
                   &bytes_witten, nullptr))
    {
        if (GetLastError() == ERROR_BROKEN_PIPE)
        {
            std::cout << "WARNING: The error ERROR_BROKEN_PIPE happened while "
                         "writing."
                      << std::endl;
        }
        else
        {
            std::cerr << "ERROR: InstanceThread WriteFile failed, GLE= "
                      << GetLastError() << std::endl;
        }
        return false;
    }
    else if ((DWORD)response.size() + 1 != bytes_witten)
    {
        std::cerr << "ERROR: The amount of data written in the pipe is not "
                     "what is supposed to be."
                  << std::endl;
        return false;
    }
    return true;
}