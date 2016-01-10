#ifndef _CMNT_COMMENT_EDITOR_HPP_
#define _CMNT_COMMENT_EDITOR_HPP_

#include <string>

std::string new_comment_from_file(std::string filename);
std::string update_comment_from_file(std::string filename);
void display_comment(std::string comment);

#endif
