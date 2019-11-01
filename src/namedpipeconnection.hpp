#pragma once

#include "connection.hpp"
#include "handler.hpp"

#include <windows.h>

/**
 * @brief This class is in charge to make control the infrastructure between the
 * server and the namedpipe. It inherits from AbstractConnection in order to
 * hide the implementation of it from the Server.
 *
 */
class NamedPipeConnection : public AbstractConnection
{
  public:
    /**
     * @brief Construct a new Named Pipe Connection object.
     *
     * @param handler It is able to handle all the request from the clients.
     */
    NamedPipeConnection(AbstractHandler & handler);

    void connect() override;

  private:
    AbstractHandler & m_handler;

    void start_named_pipe_worker();

    void instance_thread(const HANDLE pipe) const;
};