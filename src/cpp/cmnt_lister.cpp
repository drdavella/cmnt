#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <cstring>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <cmnt_lister.hpp>


int get_dir_listing(const char * dirname, sort_type_t sort, bool list_all)
{
    DIR * dirp;
    struct dirent * dp;
    struct stat statbuf;

    if ((dirp = opendir(dirname)) == NULL)
    {
        return -1;
    }

    // list all of the files in the directory
    std::string dirstring = dirname;
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

        // I think I want lstat (don't follow links)...
        if (lstat(fullpath.c_str(),&statbuf) != 0)
        {

        }
    }

    closedir(dirp);


   return 0;
}
