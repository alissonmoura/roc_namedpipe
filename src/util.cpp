#include "util.hpp"

static constexpr auto ENVVAR_NAMEDPIPE_PATH = "NAMED_PIPE_PATH";
static constexpr auto DEFAULT_NAMEDPIPE_PATH = "\\\\.\\pipe\\mynamedpipe";

std::string util::get_namedpipe_path()
{
    const char * namedpipepath = std::getenv(ENVVAR_NAMEDPIPE_PATH);
    if (!namedpipepath)
    {
        namedpipepath = DEFAULT_NAMEDPIPE_PATH;
    }
    return std::string(namedpipepath);//move semantics implicitly
}
