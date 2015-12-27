#ifndef _CMNT_LISTER_HPP_
#define _CMNT_LISTER_HPP_

#define LISTER_ERROR    (-1)


typedef enum sort_type
{
    DEFAULT_SORT,
    MODTIME,
    NAME_REVERSE,
    MODTIME_REVERSE
} sort_type_t;


int print_dir_listing(const char * dirname, bool long_list=false,
                      bool list_all=false, sort_type_t sort_type=DEFAULT_SORT);


#endif
