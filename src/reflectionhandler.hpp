#pragma once

#include "handler.hpp"
#include "objects/object.hpp"

#include <rttr/registration>

#include <memory>
#include <mutex>
#include <unordered_map>


struct RequestMessage{

public:
    std::string type;
    std::string object_type;
    std::string func;
    std::vector<rttr::argument> args;
    int obj_id{};
    int client_id{};
    RequestMessage(const std::string& json);

private:
    std::vector<std::unique_ptr<std::string>> m_str_ptrs;
    void build_request(const std::string& json);
    void add_string_to_args(const char* str, const size_t size);
};

class ReflectionHandler : public AbstractHandler
{
public:
	ReflectionHandler();
	std::string handle(const std::string& json) override;
private:
	std::unordered_map<std::string, std::unique_ptr<AbstractRepo>> m_repos;
	std::mutex m_mutex;

	void init_repos();
	bool contains_key(const std::string& key);
	static std::string build_json(const rttr::variant& var, const RequestMessage& msg);

};


