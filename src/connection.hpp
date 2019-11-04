#pragma once

/**
 * @brief It is an interface that has contract connect.
 * It is done this way so the Server doesn't know any implementation of the
 * classes that inherits it.
 *
 */
class AbstractConnection
{
  public:
    virtual ~AbstractConnection() {}
    virtual bool connect() = 0;
};
