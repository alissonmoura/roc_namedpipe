#pragma once

#include <string>

/**
 * @brief It is an interface that has the contract handle.
 * It is done this way so the NamedPipeConnection doesn't know any
 * implementation of the classes that inherits it.
 *
 */
class AbstractHandler
{
  public:
    virtual ~AbstractHandler() {}
    virtual std::string handle(const std::string & msg) = 0;
};
