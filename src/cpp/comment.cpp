#include <cstring>
#include <error.h>
#include <errno.h>
#include <comment.hpp>

#define CMNT_LEN_FIELD  "user.cmnt-len"
#define CMNT_MSG_FIELD  "user.cmnt-msg"


int add_comment(const std::string path, const std::string comment)
{
    size_t len = comment.size();
    // currently creates new, or replaces if already exists
    if (setxattr(path.c_str(),CMNT_LEN_FIELD,&len,sizeof(len),0) != 0)
    {
        fprintf(stderr,"error adding length field: %s\n",strerror(errno));
        return errno;
    }
    if (setxattr(path.c_str(),CMNT_MSG_FIELD,comment.c_str(),len,0) != 0)
    {
        fprintf(stderr,"error adding comment field: %s\n",strerror(errno));
        return errno;
    }
    return 0;
}

int get_comment(std::string &message, const std::string path)
{
    size_t len;
    if (getxattr(path.c_str(),CMNT_LEN_FIELD,&len,sizeof(len)) <= 0)
    {
        if (errno == ENODATA)
        {
            return NO_COMMENT;
        }
        else
        {
            fprintf(stderr,"error reading length field: %s\n",strerror(errno));
            return errno;
        }
    }

    char msg[len + 1];
    msg[len] = 0;
    if ((size_t) getxattr(path.c_str(),CMNT_MSG_FIELD,msg,len) != len)
    {
        fprintf(stderr,"error reading message field: %s\n",strerror(errno));
        return errno;
    }
    message = msg;
    return 0;
}
