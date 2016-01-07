#ifndef _CMNT_COMMENT_HPP_
#define _CMNT_COMMENT_HPP_

#include <string>
#include <sys/xattr.h>

#define NO_COMMENT  ENODATA

bool has_comment(std::string path);
int add_comment(const std::string path, const std::string comment,
                bool force=false);
int get_comment(std::string &message, const std::string path);
int remove_comment(const std::string path);


#endif
