#define CATCH_CONFIG_MAIN

#include "../src/reflectionhandler.hpp"

#include <rttr/variant.h>
#include <catch.hpp>


TEST_CASE("json_to_msg", "Message") {

	const char json[] = "{\"type\": \"object\", \
                          \"object_type\": \"Car\", \
                          \"func\": \"change_color\", \
                          \"args\": [\"blue\"], \
						  \"obj_id\": 1,  \"client_id\": 1}";

	RequestMessage msg(json);

	REQUIRE(msg.type == "object");
	REQUIRE(msg.object_type == "Car");
	REQUIRE(msg.func == "change_color");
	REQUIRE(msg.args[0].get_value<std::string>() == "blue");
	REQUIRE(msg.obj_id == 1);
	REQUIRE(msg.client_id == 1);
}

TEST_CASE("handle_procedure", "ServerTest") {
	const std::string json = "{\"type\": \"procedure\", \
                               \"object_type\": \"\", \
                               \"func\": \"project_name\", \
                               \"args\": [\"\"], \
                               \"obj_id\": -1, \
                               \"client_id\": -1 }";
	ReflectionHandler handler;
	const std::string return_val = handler.handle(json);

	REQUIRE(return_val.find("roc") != std::string::npos);
}


TEST_CASE("handle_create_car_already_exists", "ServerTest") {
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

TEST_CASE("handle_create_bike_already_exists", "ServerTest") {
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

TEST_CASE("handle_car", "ServerTest") {
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

TEST_CASE("handle_bike", "ServerTest") {
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
										    \"client_id\": 1 }"; \
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

