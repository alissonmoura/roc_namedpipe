#pragma once

#include <rttr/type>


struct Object { 
	virtual ~Object() {}
	virtual std::string to_json(const int id) = 0;
	RTTR_ENABLE()
};

class AbstractRepo
{
public:
	virtual ~AbstractRepo() {}
    virtual bool create_object(const int obj_id) = 0;
    virtual Object& get_object(const int obj_id) = 0;
};
