#ifndef _FILE_DATA_HPP_
#define _FILE_DATA_HPP_

#define SIX_MONTHS_SECS     (30 * 60 * 6)
#define TIME_CHAR_BUF_SIZE  120

#include <ctime>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <map>
#include <functional>


typedef std::map<gid_t,std::string> gid_map_t;
typedef std::map<uid_t,std::string> uid_map_t;


class FileData
{
    public:
        FileData(std::string name, std::string path,
                 struct stat statdata, gid_map_t& gid_map,
                 uid_map_t& uid_map, time_t now,
                 bool long_list=false);
        void write_to_stream(std::ostream& stream, std::string format,
                             bool color=false);
        size_t name_len;
        size_t owner_len;
        size_t group_len;
        size_t size_len;
    private:
        std::string color_name_by_type(void);
        std::string name;
        std::string path;
        std::string owner;
        std::string group;
        std::string modtime_string;
        std::string mode_string;
        off_t num_bytes;
        time_t modtime;
        mode_t file_type;
        static std::map<gid_t,std::string> gid_map;
        static std::map<uid_t,std::string> uid_map;
    public:
        static std::function<bool (FileData, FileData)>
            compare_by_name(bool reverse=false);
        static std::function<bool (FileData, FileData)>
            compare_by_modtime(bool reverse=false);
};


#endif
