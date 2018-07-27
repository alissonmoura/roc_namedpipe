#include "procedures.hpp"

#include <rttr/registration>

std::string return_text(std::string& text)
{
    return text;
}

std::string project_name()
{
	return "roc";
}

RTTR_REGISTRATION
{
	rttr::registration::method("return_text", &return_text)
				       .method("project_name", &project_name);
}
