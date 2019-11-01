#include "../src/server.hpp"
#include "../src/reflectionhandler.hpp"
#include "../src/namedpipeconnection.hpp"

int main()
{
    ReflectionHandler handler;
    NamedPipeConnection connection(handler);
    Server server(connection);
    server.run();
    return 0;
}