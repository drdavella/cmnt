#ifndef _FILE_DATA_HPP_
#define _FILE_DATA_HPP_

#define SIX_MONTHS_SECS     (30 * 60 * 6)
#define TIME_CHAR_BUF_SIZE  120

#include <ctime>
#include <boost/date_time.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <map>

typedef std::map<gid_t,std::string> gid_map_t;
typedef std::map<uid_t,std::string> uid_map_t;


class FileData
{
    public:
        FileData(std::string path, struct stat statdata,
                 gid_map_t& gid_map, uid_map_t& uid_map,
                 time_t now);

    private:
        std::string path;
        std::string owner;
        std::string group;
        std::string modtime_string;
        time_t modtime;
        static std::map<gid_t,std::string> gid_map;
        static std::map<uid_t,std::string> uid_map;
};


FileData::FileData(std::string path, struct stat statdata,
                   gid_map_t& gid_map, uid_map_t& uid_map,
                   time_t now)
{
    struct group * grpbuf;
    struct passwd * nambuf;
    char strbuf[TIME_CHAR_BUF_SIZE + 1] = { 0 };
    std::string time_format;
    this->path = path;

    // cache the group name to avoid needless syscalls
    if (gid_map.find(statdata.st_gid) == gid_map.end())
    {
        grpbuf = getgrgid(statdata.st_gid);
        group = grpbuf->gr_name;
        gid_map[statdata.st_gid] = group;
    }
    else
    {
        group = gid_map[statdata.st_gid];
    }
    // cache the user name to avoid needless syscalls
    if (uid_map.find(statdata.st_uid) == uid_map.end())
    {
        nambuf = getpwuid(statdata.st_uid);
        owner = nambuf->pw_name;
        uid_map[statdata.st_uid] = owner;
    }
    else
    {
        owner = uid_map[statdata.st_uid];
    }

    // modification time, which is what ls uses for sorting
    modtime = statdata.st_mtim.tv_sec;
    if (modtime - now > SIX_MONTHS_SECS)
    {
        time_format = "%b %e %Y";
    }
    else
    {
        time_format = "%b %e %H:%M";
    }
    strftime(strbuf,TIME_CHAR_BUF_SIZE,time_format.c_str(),localtime(&modtime));
    modtime_string = strbuf;
    std::cout << modtime_string << std::endl;
}


#endif
