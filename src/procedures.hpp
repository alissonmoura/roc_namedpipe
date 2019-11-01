#pragma once

#include <string>

/**
 * @brief It is only an example of how a procedure can be called.
 * It return its own argument
 *
 * @param text
 * @return std::string its own argument
 */
std::string return_text(std::string & text);

/**
 * @brief It is only an example of how a procedure can be called.
 * It returns the name of the project roc
 *
 * @return std::string roc
 */
std::string project_name();