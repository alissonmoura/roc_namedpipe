#pragma once

#include <rttr/type>

/**
 * @brief All the objects need to inherit from it to be an object that can be called from the client to the server.
 * 
 */
struct Object
{
    virtual ~Object() {}
    /**
     * @brief Transforms the object in JSON.
     * 
     * @param id The object id.
     * @return std::string The JSON.
     */
    virtual std::string to_json(const int id) const = 0;
    // it enables reflectrion
    RTTR_ENABLE()
};

/**
* @brief This Interface has the contract of all the repos
* 
*/
class AbstractRepo
{
  public:
    virtual ~AbstractRepo() {}
    virtual bool create_object(const int obj_id) = 0;
    virtual Object & get_object(const int obj_id) = 0;
};
