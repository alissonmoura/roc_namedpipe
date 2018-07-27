add_executable(
    server-test
	server_test.cpp
    ../src/objects/car.cpp
    ../src/objects/bike.cpp
	../src/reflectionhandler.cpp
	../src/procedures.cpp
	../src/namedpipeconnection.cpp
	../src/server.cpp
)

find_package(RTTR CONFIG REQUIRED Core)

target_include_directories(server-test PUBLIC
    ../vendor/rttr/include
    ../vendor/rapidjson/include
    )

target_link_libraries(server-test PUBLIC RTTR::Core_Lib)

add_executable(
    client-test
	../src/client.cpp
	client_test.cpp
)