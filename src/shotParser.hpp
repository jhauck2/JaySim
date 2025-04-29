#ifndef __SHOT_PARSER__
#define __SHOT_PARSER__


#include "shotData.hpp"
#include <string>

t_shot_data parse_json_shot_string(const std::string& shot_string);
t_shot_data parse_json_shot_file(const std::string& path);


#endif