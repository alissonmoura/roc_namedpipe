#include "../src/server.hpp"
#include "../src/reflectionhandler.hpp"
#include "../src/namedpipeconnection.hpp"

#include <iostream>

int main()
{
    ReflectionHandler handler;
    NamedPipeConnection connection(handler);
    Server server(connection);

    if (!server.run())
    {
        std::cerr
            << "ERROR: Somthing wrong happened to the server it will be exited."
            << std::endl;
        std::cout << "<press ENTER to exit>" << std::endl;
        std::cin.get();
        return -1;
    }

    return 0;
}