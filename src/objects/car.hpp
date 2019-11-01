#pragma once

#include "object.hpp"

#include <rttr/type>

#include <memory>
#include <string>
#include <unordered_map>

struct Car : public Object
{
    std::string m_model;

    void change_model(std::string & model);
    std::string get_model() const;
    std::string to_json(const int id) const;

    RTTR_ENABLE()
};

class CarRepo : public AbstractRepo
{
  public:
    static const std::string REPO_NAME;

    Object & get_object(const int obj_id) override;
    bool create_object(const int obj_id) override;

  private:
    bool contains_object(const int key);

    std::unordered_map<int, std::unique_ptr<Car>> m_cars;
};
