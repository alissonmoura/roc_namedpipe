#define CATCH_CONFIG_MAIN

#include "../src/reflectionhandler.hpp"

#include <catch.hpp>
#include <rttr/variant.h>

TEST_CASE("json_to_msg", "Message")
{

    const char json[] = "{\"type\": \"object\", \
                          \"object_type\": \"Car\", \
                          \"func\": \"change_color\", \
                          \"args\": [\"blue\"], \
                          \"obj_id\": 1,  \"client_id\": 1}";

    RequestMessage msg(json);

    REQUIRE(msg.type == MessageType::OBJECT);
    REQUIRE(msg.object_type == "Car");
    REQUIRE(msg.func == "change_color");
    REQUIRE(msg.get_args()[0].get_value<std::string>() == "blue");
    REQUIRE(msg.obj_id == 1);
    REQUIRE(msg.client_id == 1);
}

TEST_CASE("json_to_msg_testing_integer", "Message")
{

    const char json[] = "{\"type\": \"object\", \
                          \"object_type\": \"Car\", \
                          \"func\": \"change_plate\", \
                          \"args\": [123123], \
                          \"obj_id\": 1,  \"client_id\": 1}";

    RequestMessage msg(json);

    REQUIRE(msg.type == MessageType::OBJECT);
    REQUIRE(msg.object_type == "Car");
    REQUIRE(msg.func == "change_plate");
    REQUIRE(msg.get_args()[0].get_value<int>() == 123123);
    REQUIRE(msg.obj_id == 1);
    REQUIRE(msg.client_id == 1);
}

TEST_CASE("handle_procedure_project_name", "ServerTest")
{
    const std::string json = "{\"type\": \"procedure\", \
                               \"object_type\": \"\", \
                               \"func\": \"project_name\", \
                               \"args\": [\"\"], \
                               \"obj_id\": -1, \
                               \"client_id\": -1 }";
    ReflectionHandler handler;
    const std::string return_val = handler.handle(json);

    REQUIRE(return_val.find("\"roc\"") != std::string::npos);
}

TEST_CASE("handle_procedure_return_text", "ServerTest")
{
    const std::string json = "{\"type\": \"procedure\", \
                               \"object_type\": \"\", \
                               \"func\": \"return_text\", \
                               \"args\": [\"word\"], \
                               \"obj_id\": -1, \
                               \"client_id\": -1 }";
    ReflectionHandler handler;
    const std::string return_val = handler.handle(json);

    REQUIRE(return_val.find("\"word\"") != std::string::npos);
}

TEST_CASE("handle_procedure_return_text_error_no_arg", "ServerTest")
{
    const std::string json = "{\"type\": \"procedure\", \
                               \"object_type\": \"\", \
                               \"func\": \"return_text\", \
                               \"args\": [], \
                               \"obj_id\": -1, \
                               \"client_id\": -1 }";
    ReflectionHandler handler;
    const std::string return_val = handler.handle(json);

    REQUIRE(return_val ==
            "{\"response\":\"ERROR: function return_text was not found!\"}");
}

TEST_CASE("handle_procedure_return_text_error_more_args", "ServerTest")
{
    const std::string json = "{\"type\": \"procedure\", \
                               \"object_type\": \"\", \
                               \"func\": \"return_text\", \
                               \"args\": [\"word1\", \"word2\"], \
                               \"obj_id\": -1, \
                               \"client_id\": -1 }";
    ReflectionHandler handler;
    const std::string return_val = handler.handle(json);

    REQUIRE(return_val == "{\"response\":\"ERROR: function return_text with "
                          "args (word1, word2, ) was not found!\"}");
}

TEST_CASE("handle_procedure_project_name_error_typo", "ServerTest")
{
    const std::string json = "{\"type\": \"procedure\", \
                               \"object_type\": \"\", \
                               \"func\": \"project_nam\", \
                               \"args\": [\"\"], \
                               \"obj_id\": -1, \
                               \"client_id\": -1 }";
    ReflectionHandler handler;
    const std::string return_val = handler.handle(json);

    REQUIRE(return_val ==
            "{\"response\":\"ERROR: function project_nam was not found!\"}");
}

TEST_CASE("handle_create_car_already_exists", "ServerTest")
{
    ReflectionHandler handler;

    const std::string json = "{\"type\": \"create\", \
                               \"object_type\": \"Car\", \
                               \"func\": \"create\", \
                               \"args\": [\"\"], \
                               \"obj_id\": 1, \
                               \"client_id\": 1 }";

    const auto return_val_first_call = handler.handle(json);
    const auto return_val_second_call = handler.handle(json);

    REQUIRE(return_val_first_call.find("true") != std::string::npos);
    REQUIRE(return_val_second_call.find("false") != std::string::npos);
}

TEST_CASE("handle_create_bike_already_exists", "ServerTest")
{
    ReflectionHandler handler;

    const std::string json = "{\"type\": \"create\", \
                               \"object_type\": \"Bike\", \
                               \"func\": \"create\", \
                               \"args\": [\"\"], \
                               \"obj_id\": 1, \
                               \"client_id\": 1 }";

    const auto return_val_first_call = handler.handle(json);
    const auto return_val_second_call = handler.handle(json);

    REQUIRE(return_val_first_call.find("true") != std::string::npos);
    REQUIRE(return_val_second_call.find("false") != std::string::npos);
}

TEST_CASE("handle_car", "ServerTest")
{
    ReflectionHandler handler;

    const std::string json_create = "{\"type\": \"create\", \
                                      \"object_type\": \"Car\", \
                                      \"func\": \"create\", \
                                      \"args\": [\"\"], \
                                      \"obj_id\": 1, \
                                      \"client_id\": 1 }";
    std::string return_val = handler.handle(json_create);

    const std::string json_change_model = "{\"type\": \"object\", \
                                            \"object_type\": \"Car\", \
                                            \"func\": \"change_model\", \
                                            \"args\": [\"Beetle\"], \
                                            \"obj_id\": 1, \
                                            \"client_id\": 1 }";
    return_val = handler.handle(json_change_model);

    const std::string json_get_model = "{\"type\": \"object\", \
                                         \"object_type\": \"Car\", \
                                         \"func\": \"get_model\", \
                                         \"args\": [\"\"], \
                                         \"obj_id\": 1, \
                                         \"client_id\": 1 }";
    return_val = handler.handle(json_get_model);

    REQUIRE(return_val.find("Beetle") != std::string::npos);
}

TEST_CASE("handle_bike", "ServerTest")
{
    ReflectionHandler handler;

    const std::string json_create = "{\"type\": \"create\", \
                                      \"object_type\": \"Bike\", \
                                      \"func\": \"create\", \
                                      \"args\": [\"\"], \
                                      \"obj_id\": 1, \
                                      \"client_id\": 1 }";
    handler.handle(json_create);
    const std::string json_change_color = "{\"type\": \"object\", \
                                            \"object_type\": \"Bike\", \
                                            \"func\": \"change_color\", \
                                            \"args\": [\"blue\"], \
                                            \"obj_id\": 1, \
                                            \"client_id\": 1 }";
    handler.handle(json_change_color);
    const std::string json_get_model = "{\"type\": \"object\", \
                                         \"object_type\": \"Bike\", \
                                         \"func\": \"get_color\", \
                                         \"args\": [\"\"], \
                                         \"obj_id\": 1, \
                                         \"client_id\": 1 }";
    const auto return_val = handler.handle(json_get_model);

    REQUIRE(return_val.find("blue") != std::string::npos);
}

TEST_CASE("handle_car_error_create_typo", "ServerTest")
{
    ReflectionHandler handler;

    const std::string json_create = "{\"type\": \"creat\", \
                                      \"object_type\": \"Car\", \
                                      \"func\": \"create\", \
                                      \"args\": [\"\"], \
                                      \"obj_id\": 1, \
                                      \"client_id\": 1 }";
    std::string return_val = handler.handle(json_create);

    REQUIRE(return_val ==
            "{\"response\":\"ERROR: creat is not an acceptable type\"}");
}

TEST_CASE("handle_car_error_object_type_doesnt_exist", "ServerTest")
{
    ReflectionHandler handler;

    const std::string json_create = "{\"type\": \"create\", \
                                      \"object_type\": \"House\", \
                                      \"func\": \"create\", \
                                      \"args\": [\"\"], \
                                      \"obj_id\": 1, \
                                      \"client_id\": 1 }";
    std::string return_val = handler.handle(json_create);

    REQUIRE(return_val ==
            "{\"response\":\"ERROR: Object Type House not found.\"}");
}

TEST_CASE("handle_not_created", "Message")
{

    const std::string json = "{\"type\": \"object\", \
                          \"object_type\": \"Car\", \
                          \"func\": \"change_plat\", \
                          \"args\": [123123], \
                          \"obj_id\": 1,  \"client_id\": 1}";

    ReflectionHandler handler;
    std::string return_val = handler.handle(json);

    REQUIRE(return_val ==
            "{\"response\":\"ERROR: Object 1 not found!\"}");
}

TEST_CASE("handle_bike_mothod_typo", "ServerTest")
{
    ReflectionHandler handler;

    const std::string json_create = "{\"type\": \"create\", \
                                      \"object_type\": \"Bike\", \
                                      \"func\": \"create\", \
                                      \"args\": [\"\"], \
                                      \"obj_id\": 1, \
                                      \"client_id\": 1 }";
    handler.handle(json_create);
    const std::string json_change_color = "{\"type\": \"object\", \
                                            \"object_type\": \"Bike\", \
                                            \"func\": \"change_colo\", \
                                            \"args\": [\"blue\"], \
                                            \"obj_id\": 1, \
                                            \"client_id\": 1 }";
    std::string return_val = handler.handle(json_change_color);

    REQUIRE(return_val == "{\"response\":\"ERROR: function change_colo with args (blue, ) was not found!\"}");
}

TEST_CASE("handle_bike_mothod_no_args", "ServerTest")
{
    ReflectionHandler handler;

    const std::string json_create = "{\"type\": \"create\", \
                                      \"object_type\": \"Bike\", \
                                      \"func\": \"create\", \
                                      \"args\": [\"\"], \
                                      \"obj_id\": 1, \
                                      \"client_id\": 1 }";
    handler.handle(json_create);
    const std::string json_change_color = "{\"type\": \"object\", \
                                            \"object_type\": \"Bike\", \
                                            \"func\": \"change_color\", \
                                            \"args\": [], \
                                            \"obj_id\": 1, \
                                            \"client_id\": 1 }";
    std::string return_val = handler.handle(json_change_color);

    REQUIRE(return_val == "{\"response\":\"ERROR: function change_color was not found!\"}");
}

TEST_CASE("handle_bike_mothod_more_args", "ServerTest")
{
    ReflectionHandler handler;

    const std::string json_create = "{\"type\": \"create\", \
                                      \"object_type\": \"Bike\", \
                                      \"func\": \"create\", \
                                      \"args\": [\"\"], \
                                      \"obj_id\": 1, \
                                      \"client_id\": 1 }";
    handler.handle(json_create);
    const std::string json_change_color = "{\"type\": \"object\", \
                                            \"object_type\": \"Bike\", \
                                            \"func\": \"change_colo\", \
                                            \"args\": [\"blue\", \"red\"], \
                                            \"obj_id\": 1, \
                                            \"client_id\": 1 }";
    std::string return_val = handler.handle(json_change_color);

    REQUIRE(return_val == "{\"response\":\"ERROR: function change_colo with "
                          "args (blue, red, ) was not found!\"}");
}
