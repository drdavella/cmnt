#ifndef _FILEDATALIST_HPP_
#define _FILEDATALIST_HPP_

#include <iostream>
#include <vector>
#include <FileData.hpp>


typedef enum sort_type
{
    DEFAULT_SORT,
    MODTIME,
    NAME_REVERSE,
    MODTIME_REVERSE
} sort_type_t;


class FileDataList
{
    public:
        FileDataList(bool long_list=false) { this->long_list = long_list; }
        void add_data(FileData data);
        void sort_by(sort_type_t sort);
        std::string create_format_string(size_t term_width);
        void list_data(std::ostream& stream, int term_width, bool color=false);

    private:
        std::vector<FileData> data;
        bool long_list;
        size_t longest_name = 0;
        size_t longest_owner = 0;
        size_t longest_group = 0;
        size_t longest_size = 0;
};


#endif
