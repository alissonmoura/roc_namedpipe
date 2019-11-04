#pragma once

#include <string>

namespace util
{
/**
 * @brief Get the namedpipe path. It will get the namedpipe path from the
 * environment variable NAMED_PIPE_PATH. If it doesn't exist it will use the
 * path: \\\\.\\pipe\\mynamedpipe
 *
 * @return std::string It returns the namedpipe path as string. The move
 * semantics is done implicitly.
 */
std::string get_namedpipe_path();
constexpr auto BUFSIZE = 4096;
} // namespace util
