#pragma once

#include "object.hpp"

#include <rttr/type>

#include <memory>
#include <string>
#include <unordered_map>

/**
 * @brief This is the Object Bike that can be called from the client.
 * 
 */
struct Bike : public Object
{
    std::string m_color;

    void change_color(std::string & color);
    std::string get_color() const;
    std::string to_json(const int id) const;
    RTTR_ENABLE()
};


/**
 * @brief This is the repository of bikes.
 * It holds all the references to the bikes.
 * 
 */
class BikeRepo : public AbstractRepo
{
  public:
    static const std::string REPO_NAME;
    Object & get_object(const int obj_id) override;
    bool create_object(const int obj_id) override;

  private:
    bool contains_object(const int key);

    std::unordered_map<int, std::unique_ptr<Bike>> m_bikes;
};
