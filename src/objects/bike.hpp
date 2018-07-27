#pragma once

#include "object.hpp"

#include <rttr/type>

#include <string>
#include <unordered_map>
#include <memory>

struct Bike : public Object
{
    std::string m_color;

    void change_color(std::string& color);
    std::string get_color() const;
	std::string to_json(const int id) override;
    RTTR_ENABLE()
};

class BikeRepo : public AbstractRepo
{
public:
    Object& get_object(const int obj_id) override;
    bool create_object(const int obj_id) override;

private:
    bool contains_key(const int key);

    std::unordered_map<int, std::unique_ptr<Bike>> m_bikes;
};
