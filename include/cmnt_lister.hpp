#ifndef _CMNT_LISTER_HPP_
#define _CMNT_LISTER_HPP_

typedef enum sort_type
{
    DEFAULT_SORT,
    // when sorting by time, ls uses modification time

} sort_type_t;


int get_dir_listing(const char * dirname, sort_type_t sort=DEFAULT_SORT,
                    bool list_all=false);


#endif
