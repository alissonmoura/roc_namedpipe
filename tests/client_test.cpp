#include <random>
#include <iostream>
#include <thread>
#include <sstream>

#include "../src/client.hpp"



int main()
{
	std::random_device rd;
	std::mt19937 mt(rd());
	const std::uniform_int_distribution<> dist(1, 1000);
	const int obj_id = dist(mt);

	Client client;
	client.connect();

	//calling a procedure
	std::ostringstream json_procedure;

	json_procedure << "{\"type\": \"procedure\", "
		<< "\"object_type\": \"\", "
		<< "\"func\": \"project_name\", "
		<< "\"args\": [], "
		<< "\"obj_id\": 1, "
		<< "\"client_id\": 1 }";

	//sync_call
	const std::string ret_procedure = client.sync_call(json_procedure.str());
	std::cout << std::endl << std::endl;
	std::cout << "returning name of the project using procedure" << std::endl;
	std::cout << ret_procedure << std::endl;
	std::cout << std::endl << std::endl;

	std::ostringstream json_create;
	json_create << "{\"type\": \"create\", "
		        << "\"object_type\": \"Car\", "
		        << "\"func\": \"create\", "
		        << "\"args\": [\"\"], "
		        << "\"obj_id\": " << obj_id << ", "
		        << "\"client_id\": 1 }";

	std::cout << "calling method create" << std::endl;

	//sync_call
	const std::string ret_create = client.sync_call(json_create.str());

	std::cout << std::endl << std::endl;
	std::cout << "return for create" << std::endl;
	std::cout << ret_create << std::endl;
	std::cout << std::endl << std::endl;

	std::ostringstream json_change_model;
	json_change_model << "{\"type\": \"object\","
		              << "\"object_type\": \"Car\","
		              << "\"func\": \"change_model\","
		              << "\"args\": [\"Beetle\"],"
		              << "\"obj_id\":" << obj_id << ","
		              << "\"client_id\": 1 }";

	std::cout << "calling method change_model asynchronously" << std::endl;
	std::cout << std::endl << std::endl;

	//async_call
	std::future<std::string> ret_change_model = client.async_call(json_change_model.str());

	std::ostringstream json_get_model;
	json_get_model << "{\"type\": \"object\","
		           << "\"object_type\": \"Car\","
		           << "\"func\": \"get_model\","
		           << "\"args\": [\"\"],"
		           << "\"obj_id\":" << obj_id << ","
		           << "\"client_id\": 1 }";


	//sync_call
	std::string ret_get_model = client.sync_call(json_get_model.str());

	std::cout << "calling method get_model" << std::endl;
	std::cout << std::endl << std::endl;

	std::cout << "I just called the change model asynchronously. \
 It should have changed the model of the car to Beetle but \
as you can see in the next call it is possible to happen or not. \
Probably the std::async takes more time to be invoked than the next call get_model  " << std::endl; 

	std::cout << std::endl << std::endl;
	std::cout << "return for get_model" << std::endl;
	std::cout << ret_get_model << std::endl;
	std::cout << std::endl << std::endl;

	ret_change_model.get();
	std::cout << "At this point the change_model was already processed so if \
I call get_model again it should return Beetle." << std::endl;

	//sync_call
	ret_get_model = client.sync_call(json_get_model.str());

	std::cout << std::endl << std::endl;
	std::cout << "return for second get_model" << std::endl;
	std::cout << ret_get_model << std::endl;
	std::cout << std::endl << std::endl;


	std::cout << std::endl << std::endl;
	std::cout << "Finally, I am going to retrieve the whole object" << std::endl;
	std::cout << std::endl << std::endl;

	std::ostringstream json_retrieve;
	json_retrieve << "{\"type\": \"retrieve\", "
		<< "\"object_type\": \"Car\", "
		<< "\"func\": \"retrieve\", "
		<< "\"args\": [\"\"], "
		<< "\"obj_id\": " << obj_id << ", "
		<< "\"client_id\": 1 }";

	//sync_call
	std::string ret_retrieve = client.sync_call(json_retrieve.str());

	std::cout << std::endl << std::endl;
	std::cout << ret_retrieve << std::endl;
	std::cout << std::endl << std::endl;

	
	std::cout << "<End of message, press ENTER to terminate connection and exit>"  << std::endl;
	std::cin.get();
	client.close();

	return 0;
}