# ROC - Remote Object Call

ROC is a server/client application which enables the clients to create and manipulate the objects created in the server. It is also possible to call some procedures in the server.
Features:

 - Create an object in the server
 - Retrieve an object from the server
 - Call a method on the server and get its return
 - Call a procedure in the server
 - All of this functions can be done synchronously and asynchronously

## Design

![ROC](https://github.com/alissonmoura/roc_namedpipe/blob/master/doc/roc_design.png)

### Client
The client is able to connect to a namedpipe and send JSON strings to it using sync and async methods.
The async call was implemented using the async implemented in the standard C++ library. It was chosen because it is a simple way to have async methods in C++ with not so much code. Also, it is easy to maintain.
The choice to use JSON to be the data type of the communication was because it is very simple to manipulate and it is also very flexible to work on both sides (client and server)

### Server
The server accepts multiple clients using a thread for each client. The calls are handled using these 3 main ideas:

 - The Connection
 - The Handler
 - The Registered Objects

The Connection is the connection between the Server and the namedpipe. The way I designed the code I have an AbstractConnection (Interface) which is passed as a parameter to the Server object. In the Server object, there is a run method that calls connect from the AbstractConnection. Therefore the server doesn’t know what kind of connection it is using what makes the server more flexible to implement future connections.

The Handler is what knows what to do with the message coming from the Connection. So the connection has an AbstractHandler (Interface) that is responsible to handle the messages. The Handler can be implemented in many ways but for now, I use Reflection to call the methods of the objects created in the server. I thought about using Reflection because since the method called is known only during runtime I had to find a way to call the method dynamically. It is a good tool for the job because it keeps the code simple, for instance: in order to put a new kind of object in the server, it is only necessary to change 1 line in the Handler and the object itself in a new file. However, C++ doesn’t support reflection in its standard library so I used a library called RTTR that works very well with reflection.

The Registered Objects is where the developer will register all objects that it wants to have in the server side. It is very simple have it, just implement two class from Object and AbstractRepo and it is pretty much done.
The only new place that it is necessary to change is one line of the reflectionhandler.cpp to add the new repo in the member m_repos that keeps all the repos.


## Testing
In order to test it just run the server-test.exe and after that run the client-test.exe (both in the bin folder). The Server supports many clients so you can run as many clients as you want.


## Building
Set RTTR_DIR as a variable environment for the RTTR library. e.g for Windows:
set RTTR_DIR=c:/somepath/rttr
Open the project in Visual Studio and use CMake to compile.

Remarks:
I used Visual Studio 2017 - version 15.7.5 - MSVC 14.14.26428 to build the project.
The rttr library contained in this project is rttr-0.9.5-win64-vs2015. If you need another version you can download it here https://www.rttr.org/download
