#include <iostream>
#include <random>
#include <sstream>
#include <thread>

#include "../src/client.hpp"

static const int get_object_id();
static void call_procedure(Client & client);
static void create_car(Client & client, const int obj_id);
static std::future<std::string> change_model_async(Client & client,
                                                   const int obj_id);
static std::string get_model(Client & client, const int obj_id);
static void async_explain();
static void check_if_model_has_changed(const std::string & ret_get_model);
static void force_model_change(std::future<std::string> & ret_change_model);
static void retrieve_object(Client & client, const int obj_id);
static void retrieve_explain();
static void end_text();

int main()
{
    Client client{};
    if (!client.connect())
    {
        end_text();
        return -1;
    }

    call_procedure(client);

    const int obj_id = get_object_id();
    create_car(client, obj_id);
    auto & ret_change_model = change_model_async(client, obj_id);
    std::string ret_get_model = get_model(client, obj_id);

    async_explain();

    check_if_model_has_changed(ret_get_model);

    force_model_change(ret_change_model);

    ret_get_model = get_model(client, obj_id);
    std::cout << "####   second time   ####" << std::endl;
    check_if_model_has_changed(ret_get_model);

    retrieve_explain();
    retrieve_object(client, obj_id);

    end_text();

    client.close();

    return 0;
}

static const int get_object_id()
{
    std::random_device rd;
    std::mt19937 mt(rd());
    const std::uniform_int_distribution<> dist(1, 1000);
    return dist(mt);
}

static void call_procedure(Client & client)
{
    // calling a procedure
    std::ostringstream json_procedure;

    json_procedure << "{\"type\": \"procedure\", "
                   << "\"object_type\": \"\", "
                   << "\"func\": \"project_name\", "
                   << "\"args\": [], "
                   << "\"obj_id\": 1, "
                   << "\"client_id\": 1 }";

    // sync_call
    const std::string ret_procedure = client.sync_call(json_procedure.str());
    std::cout << std::endl << std::endl;
    std::cout << "Returning name of the project using procedure" << std::endl;
    std::cout << ret_procedure << std::endl;
    std::cout << std::endl << std::endl;
}

static void create_car(Client & client, const int obj_id)
{
    std::ostringstream json_create;
    json_create << "{\"type\": \"create\", "
                << "\"object_type\": \"Car\", "
                << "\"func\": \"create\", "
                << "\"args\": [\"\"], "
                << "\"obj_id\": " << obj_id << ", "
                << "\"client_id\": 1 }";

    std::cout << "Calling method create" << std::endl;

    // sync_call
    const std::string ret_create = client.sync_call(json_create.str());

    std::cout << std::endl << std::endl;
    std::cout << "Return for create" << std::endl;
    std::cout << ret_create << std::endl;
    std::cout << std::endl << std::endl;
}

static std::future<std::string> change_model_async(Client & client,
                                                   const int obj_id)
{
    std::ostringstream json_change_model;
    json_change_model << "{\"type\": \"object\","
                      << "\"object_type\": \"Car\","
                      << "\"func\": \"change_model\","
                      << "\"args\": [\"Beetle\"],"
                      << "\"obj_id\":" << obj_id << ","
                      << "\"client_id\": 1 }";

    std::cout << "Calling method change_model asynchronously" << std::endl;
    std::cout << std::endl << std::endl;

    // async_call
    std::future<std::string> ret_change_model =
        client.async_call(json_change_model.str());
    return ret_change_model; // move semantics implicitly
}
static std::string get_model(Client & client, const int obj_id)
{
    std::ostringstream json_get_model;
    json_get_model << "{\"type\": \"object\","
                   << "\"object_type\": \"Car\","
                   << "\"func\": \"get_model\","
                   << "\"args\": [\"\"],"
                   << "\"obj_id\":" << obj_id << ","
                   << "\"client_id\": 1 }";

    // sync_call
    std::string ret_get_model = client.sync_call(json_get_model.str());

    std::cout << "Calling method get_model" << std::endl;
    std::cout << std::endl << std::endl;
    return ret_get_model; // move semantics implicitly
}

static void async_explain()
{
    std::cout << "I just called the change model asynchronously. \
 It should have changed the model of the car to Beetle but \
as you can see in the next call it is possible to happen or not. \
Probably the std::async takes more time to be invoked than the next call get_model  "
              << std::endl;

    std::cout << std::endl << std::endl;
}

static void check_if_model_has_changed(const std::string & ret_get_model)
{
    std::cout << "Return for get_model" << std::endl;
    std::cout << ret_get_model << std::endl;
    std::cout << std::endl << std::endl;
}

static void force_model_change(std::future<std::string> & ret_change_model)
{
    ret_change_model.get();
    std::cout << "Force change_model to get complete."
              << std::endl << std::endl;
}

static void retrieve_object(Client & client, const int obj_id)
{
    std::ostringstream json_retrieve;
    json_retrieve << "{\"type\": \"retrieve\", "
                  << "\"object_type\": \"Car\", "
                  << "\"func\": \"retrieve\", "
                  << "\"args\": [\"\"], "
                  << "\"obj_id\": " << obj_id << ", "
                  << "\"client_id\": 1 }";

    // sync_call
    std::string ret_retrieve = client.sync_call(json_retrieve.str());

    std::cout << std::endl << std::endl;
    std::cout << ret_retrieve << std::endl;
    std::cout << std::endl << std::endl;
}

static void retrieve_explain()
{
    std::cout << std::endl << std::endl;
    std::cout << "Finally, I am going to retrieve the whole object"
              << std::endl;
    std::cout << std::endl << std::endl;
}

static void end_text()
{
    std::cout
        << "<press ENTER to exit>"
        << std::endl;
    std::cin.get();
}