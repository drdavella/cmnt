#ifndef _CMNT_COMMENT_HPP_
#define _CMNT_COMMENT_HPP_

#include <string>
#include <sys/xattr.h>

int add_comment(const std::string path, const std::string comment);
int get_comment(std::string &message, const std::string path);


#endif
