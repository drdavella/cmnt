#ifndef _CMNT_LISTER_HPP_
#define _CMNT_LISTER_HPP_

#include <string>

typedef enum sort_type
{
    DEFAULT_SORT,
    MODTIME,
    NAME_REVERSE,
    MODTIME_REVERSE
} sort_type_t;


int print_dir_listing(const char * dirname, bool long_list=false,
                      bool list_all=false);


#endif
