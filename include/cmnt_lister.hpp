#ifndef _CMNT_LISTER_HPP_
#define _CMNT_LISTER_HPP_

#include <string>
#include <FileData.hpp>
#include <FileDataList.hpp>


FileDataList * get_dir_listing(const char * dirname, bool long_list=false,
                               bool list_all=false);


#endif
