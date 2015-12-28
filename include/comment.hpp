#ifndef _CMNT_COMMENT_HPP_
#define _CMNT_COMMENT_HPP_

#include <string>
#include <sys/xattr.h>

#define NO_COMMENT  ENODATA

int add_comment(const std::string path, const std::string comment);
int get_comment(std::string &message, const std::string path);
int remove_comment(const std::string path);


#endif
