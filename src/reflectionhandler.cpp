#include "reflectionhandler.hpp"
#include "objects/car.hpp"
#include "objects/bike.hpp"

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

#include <string>
#include <sstream>

RequestMessage::RequestMessage(const std::string& json)
{
    build_request(json);
}

void RequestMessage::build_request(const std::string& json)
{
    rapidjson::Document doc;
    doc.Parse(json.c_str());
    assert(doc.IsObject());

    assert(doc["type"].IsString());
    type = doc["type"].GetString();

    assert(doc["object_type"].IsString());
    object_type = doc["object_type"].GetString();

    assert(doc["func"].IsString());
    func = doc["func"].GetString();

    std::string str;

    const auto& array = doc["args"];
    assert(array.IsArray());
    for (rapidjson::SizeType i = 0; i < array.Size(); i++) {
        if(array[i].IsInt()) {
            args.push_back(rttr::argument(rttr::variant(array[i].GetInt())));
        } else if(array[i].IsString()) {
            if(array[i].GetStringLength() > 0) {
                add_string_to_args(array[i].GetString(), (size_t)array[i].GetStringLength());
            }
        }else if (array[i].IsBool()) {
			args.push_back(rttr::argument(rttr::variant(array[i].GetBool())));
		}else if (array[i].IsDouble()) {
			args.push_back(rttr::argument(rttr::variant(array[i].GetDouble())));
		}else if (array[i].IsFloat()) {
			args.push_back(rttr::argument(rttr::variant(array[i].GetFloat())));
		}
        //TODO... implement other types
    }

    assert(doc["obj_id"].IsInt());
    obj_id = doc["obj_id"].GetInt();

    assert(doc["client_id"].IsInt());
    client_id = doc["client_id"].GetInt();
}

void RequestMessage::add_string_to_args(const char* str, const size_t size)
{
    std::ostringstream oss;
    for(size_t i = 0; i < size; i++) {
        oss << str[i];
    }
    m_str_ptrs.push_back(std::make_unique<std::string>(oss.str()));
    args.push_back(rttr::argument(*m_str_ptrs[m_str_ptrs.size() - 1]));
}

ReflectionHandler::ReflectionHandler()
{
	init_repos();
}

std::string ReflectionHandler::handle(const std::string& json)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	const RequestMessage msg(json);
	if (msg.type == "procedure") {
		const auto ret = rttr::type::invoke(msg.func.c_str(), msg.args);
		return build_json(ret, msg);
	}

	if (contains_key(msg.object_type)) {
		AbstractRepo* repo = m_repos[msg.object_type].get();

		if (msg.type == "create") {
			const auto ret = rttr::variant(repo->create_object(msg.obj_id));
			return build_json(ret, msg);
		}

		if (msg.type == "object") {
			const auto& obj = repo->get_object(msg.obj_id);
			const rttr::method meth = rttr::type::get(obj).get_method(msg.func.c_str());
			const auto ret = meth.invoke_variadic(obj, msg.args);
			return build_json(ret, msg);
		}

		if (msg.type == "retrieve") {
			auto& obj = repo->get_object(msg.obj_id);
			return obj.to_json(msg.obj_id);
		}
	}
	std::string error("Error: Object Type " + msg.object_type + " not found.");
	return build_json(rttr::variant(error), msg);

}

void ReflectionHandler::init_repos()
{
	m_repos["Car"] = std::make_unique<CarRepo>();
	m_repos["Bike"] = std::make_unique<BikeRepo>();
}

bool ReflectionHandler::contains_key(const std::string& key)
{
	return m_repos.find(key) != m_repos.end();
}

std::string ReflectionHandler::build_json(const rttr::variant& var, const RequestMessage& msg)
{
	rapidjson::StringBuffer buffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
	writer.StartObject();

	if (msg.type == "procedure") {
		writer.Key("procedure");
		writer.String(msg.func.c_str());
	} else {
		writer.Key("object_id");
		writer.Int(msg.obj_id);
		writer.Key("method");
		writer.String(msg.func.c_str());
	}

	writer.Key("response");
	const rttr::type type = var.get_type();
	if (type == rttr::type::get<bool>())
		writer.Bool(var.to_bool());
	else if (type == rttr::type::get<char>())
		writer.Bool(var.to_bool());
	else if (type == rttr::type::get<int8_t>())
		writer.Int(var.to_int8());
	else if (type == rttr::type::get<int16_t>())
		writer.Int(var.to_int16());
	else if (type == rttr::type::get<int32_t>())
		writer.Int(var.to_int32());
	else if (type == rttr::type::get<int64_t>())
		writer.Int64(var.to_int64());
	else if (type == rttr::type::get<uint8_t>())
		writer.Uint(var.to_uint8());
	else if (type == rttr::type::get<uint16_t>())
		writer.Uint(var.to_uint16());
	else if (type == rttr::type::get<uint32_t>())
		writer.Uint(var.to_uint32());
	else if (type == rttr::type::get<uint64_t>())
		writer.Uint64(var.to_uint64());
	else if (type == rttr::type::get<float>())
		writer.Double(var.to_double());
	else if (type == rttr::type::get<double>())
		writer.Double(var.to_double());
	else if (type == rttr::type::get<std::string>())
		writer.String(var.to_string().c_str());
	writer.EndObject();
	return std::string(buffer.GetString());
}

