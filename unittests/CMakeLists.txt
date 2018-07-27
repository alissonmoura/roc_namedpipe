add_executable(
    reflectionhandler-test
	reflectionhandler_test.cpp
    ../src/objects/car.cpp
    ../src/objects/bike.cpp
    ../src/reflectionhandler.cpp
    ../src/procedures.cpp
)

find_package(RTTR CONFIG REQUIRED Core)

target_include_directories(reflectionhandler-test PUBLIC
    ../vendor/catch2/
    ../vendor/rttr/include
    ../vendor/rapidjson/include
    )

target_link_libraries(reflectionhandler-test PUBLIC RTTR::Core_Lib)