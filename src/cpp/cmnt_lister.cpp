#ifndef _CMNT_LISTER_CPP_
#define _CMNT_LISTER_CPP_


#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <ctime>
#include <grp.h>
#include <pwd.h>
#include <cmnt_lister.hpp>
#include <boost/format.hpp>


static std::string mode_to_string(mode_t mode)
{
    std::string result;
    result += S_ISDIR(mode) ? "d" : "-";
    result += (mode & S_IRUSR) ? "r" : "-";
    result += (mode & S_IWUSR) ? "w" : "-";
    result += (mode & S_IXUSR) ? "x" : "-";
    result += (mode & S_IRGRP) ? "r" : "-";
    result += (mode & S_IWGRP) ? "w" : "-";
    result += (mode & S_IXGRP) ? "x" : "-";
    result += (mode & S_IROTH) ? "r" : "-";
    result += (mode & S_IWOTH) ? "w" : "-";
    result += (mode & S_IXOTH) ? "x" : "-";
    return result;
}


FileDataList * get_dir_listing(const char * dirname, bool long_list,
                               bool list_all)
{
    DIR * dirp;
    struct dirent * dp;
    struct stat statbuf;


    if ((dirp = opendir(dirname)) == NULL)
    {
        return nullptr;
    }

    FileDataList * files = new FileDataList(long_list);
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

        if (lstat(fullpath.c_str(),&statbuf) != 0)
        {
            // log the error somehow
            return nullptr;
        }

        files->add_data(FileData(dp->d_name,fullpath,statbuf,
                                 gid_map,uid_map,now));
    }

    closedir(dirp);
    return files;
}


void FileDataList::add_data(FileData data)
{
    if (data.name_len > longest_name)
    {
        longest_name = data.name_len;
    }
    if (long_list && (data.owner_len > longest_owner))
    {
        longest_owner = data.owner_len;
    }
    if (long_list && (data.group_len > longest_group))
    {
        longest_group = data.group_len;
    }
    if (long_list && (data.size_len > longest_size))
    {
        longest_size = data.size_len;
    }
    this->data.push_back(data);
}


void FileDataList::sort_by(sort_type_t sort)
{
    switch (sort)
    {
        case MODTIME:
            std::sort(data.begin(),data.end(),FileData::compare_by_modtime());
            break;
        case NAME_REVERSE:
            std::sort(data.begin(),data.end(),FileData::compare_by_name(true));
            break;
        case MODTIME_REVERSE:
            std::sort(data.begin(),data.end(),FileData::compare_by_modtime(true));
            break;
        default:
            std::sort(data.begin(),data.end(),FileData::compare_by_name());
    }
}


std::string FileDataList::create_format_string(size_t term_width)
{
    std::string format;
    if (long_list)
    {
        format = "%" + (boost::format("%ld") % longest_name).str() + "s";
    }
    else
    {
        format = "%s";
    }

    return format;
}


void FileDataList::list_data(std::ostream& stream, int term_width, bool color)
{
    std::string format = create_format_string(term_width);
    for (auto x : data)
    {
        x.write_to_stream(stream,format,color);
    }
}


FileData::FileData(std::string name, std::string path,
                   struct stat statdata, gid_map_t& gid_map,
                   uid_map_t& uid_map, time_t now,
                   bool long_list)
{
    struct group * grpbuf;
    struct passwd * nambuf;
    char strbuf[TIME_CHAR_BUF_SIZE + 1] = { 0 };
    std::string time_format;
    this->name = name;
    this->path = path;
    name_len = name.size();

    // modification time, which is what ls uses for sorting
    modtime = statdata.st_mtim.tv_sec;
    // mask off the bits indicating what kind of file this is
    file_type = statdata.st_mode & S_IFMT;

    if (long_list)
    {
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

        num_bytes = statdata.st_size;
        // modification time, which is what ls uses for sorting
        modtime = statdata.st_mtim.tv_sec;
        // we also want the string representation, for when we print
        if (modtime - now > SIX_MONTHS_SECS)
        {
            time_format = "%b %e %Y";
        }
        else
        {
            time_format = "%b %e %H:%M";
        }
        strftime(strbuf,TIME_CHAR_BUF_SIZE,time_format.c_str(),
                 localtime(&modtime));
        modtime_string = strbuf;
        mode_string = mode_to_string(statdata.st_mode);
    }
}


std::string FileData::color_name_by_type()
{
    std::string color_name;
    switch (file_type)
    {
        case S_IFLNK:
            color_name += "\x1b[0;36m";
            break;
        case S_IFDIR:
            color_name += "\x1b[0;34m";
            break;
        case S_IFBLK:
        case S_IFCHR:
            color_name += "\x1b[1;33;40m";
            break;
        // just a regular old file
        default:
            break;
    }
    color_name += name + "\x1b[0m";
    return color_name;
}


void FileData::write_to_stream(std::ostream& stream, std::string format,
                               bool color)
{
    std::string name_string;
    if (color)
    {
        name_string = color_name_by_type();
    }
    else
    {
        name_string = name;
    }
    stream << boost::format(format) % name_string <<  std::endl;


}


std::function<bool (FileData,FileData)> FileData::compare_by_name(bool reverse)
{
    if (reverse)
    {
        return [](FileData f0, FileData f1) { return f0.name > f1.name; };
    }
    else
    {
        return [](FileData f0, FileData f1) { return f0.name < f1.name; };
    }
}


std::function<bool (FileData,FileData)> FileData::compare_by_modtime(bool reverse)
{
    if (reverse)
    {
        return [](FileData f0, FileData f1) { return f0.modtime > f1.modtime; };
    }
    else
    {
        return [](FileData f0, FileData f1) { return f0.modtime < f1.modtime; };
    }
}


#endif
