#include "bike.hpp"
#include "object.hpp"

#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include <rttr/registration>

#include <iostream>

std::string Bike::get_color() const
{
    return m_color;
}

std::string Bike::to_json(const int id)
{
	rapidjson::StringBuffer buffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
	writer.StartObject();
	writer.Key("object_id");
	writer.Int(id);
	writer.Key("color");
	writer.String(m_color.c_str());
	writer.EndObject();
	return std::string(buffer.GetString());
}

void Bike::change_color(std::string& color)
{
    m_color = color;
}

RTTR_REGISTRATION
{
    rttr::registration::class_<Bike>("Bike")
         .constructor()
         .property("data", &Bike::m_color)
         .method("get_color", &Bike::get_color)
         .method("change_color", &Bike::change_color);
}

bool BikeRepo::create_object(const int obj_id)
{
    if(contains_key(obj_id)) {
        return false;
    } else {
        m_bikes[obj_id] = std::make_unique<Bike>();
        return true;
    }
}

Object& BikeRepo::get_object(const int obj_id)
{
    return *m_bikes[obj_id];
}

bool BikeRepo::contains_key(const int key)
{
	return m_bikes.find(key) != m_bikes.end();
}

