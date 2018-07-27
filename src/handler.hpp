#pragma once

#include <string>

class AbstractHandler
{
public:
	virtual ~AbstractHandler() {}
	virtual std::string handle(const std::string& msg) = 0;
};
