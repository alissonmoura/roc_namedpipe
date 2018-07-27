#include "car.hpp"
#include "object.hpp"

#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include <rttr/registration>

using namespace rttr;

void Car::change_model(std::string& model)
{
    m_model = model;
}


std::string Car::get_model() const
{
	return m_model;
}

std::string Car::to_json(const int id)
{
	rapidjson::StringBuffer buffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
	writer.StartObject();
	writer.Key("object_id");
	writer.Int(id);
	writer.Key("model");
	writer.String(m_model.c_str());
	writer.EndObject();
	return std::string(buffer.GetString());
}

RTTR_REGISTRATION
{
    registration::class_<Car>("Car")
         .constructor()
         .property("model", &Car::m_model)
         .method("change_model", &Car::change_model)
         .method("get_model", &Car::get_model);
}

bool CarRepo::create_object(const int obj_id)
{
    if(contains_key(obj_id)) {
        return false;
    } else {
        m_cars[obj_id] = std::make_unique<Car>();
        return true;
    }
}

Object& CarRepo::get_object(const int obj_id)
{
    return *m_cars[obj_id];
}

bool CarRepo::contains_key(const int key)
{
	return m_cars.find(key) != m_cars.end();
}

