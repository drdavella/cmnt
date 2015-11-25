#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <sys/types.h>
#include <sys/stat.h>
#include <grp.h>
#include <pwd.h>
#include <dirent.h>
#include <ctime>
#include <cmnt_lister.hpp>
#include <FileData.hpp>


int get_dir_listing(const char * dirname, sort_type_t sort, bool list_all)
{
    DIR * dirp;
    struct dirent * dp;
    struct stat statbuf;


    if ((dirp = opendir(dirname)) == NULL)
    {
        return -1;
    }

    std::vector<FileData> files;
    std::string dirstring = dirname;
    // use these for caching to reduce syscalls
    gid_map_t gid_map;
    uid_map_t uid_map;

    time_t now = time(NULL);

    // list all of the files in the directory
    while ((dp = readdir(dirp)) != NULL)
    {
        // ignore special files
        if (strcmp(dp->d_name,".") == 0)
        {
            continue;
        }
        if (strcmp(dp->d_name,"..") == 0)
        {
            continue;
        }
        // ignore dotfiles unless otherwise specified
        if (!list_all && (dp->d_name[0] == '.'))
        {
            continue;
        }
        std::string fullpath = dirstring + "/" + dp->d_name;
        std::cout << fullpath << std::endl;

        if (lstat(fullpath.c_str(),&statbuf) != 0)
        {

        }

        files.push_back(FileData(fullpath,statbuf,gid_map,uid_map,now));
    }

    closedir(dirp);


    return 0;
}
