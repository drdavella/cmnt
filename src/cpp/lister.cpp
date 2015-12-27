#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <error.h>
#include <errno.h>
#include <math.h>
#include <lister.hpp>

#define MAX_LINE_LEN        256     // arbitrary
#define MAX_NAME_LEN        32      // based on max Linux username
#define SIX_MONTHS_SECS     (6 * 30 * 24 * 60 * 60)
#define TIME_CHAR_BUF_SIZE  120

#define ANSI_BLUE           "\x1b[0;34m"
#define ANSI_LIGHT_CYAN     "\x1b[1;36m"
#define ANSI_LIGHT_GREEN    "\x1b[1;32m"
#define ANSI_NOTHING        "\x1b[0m"

typedef struct file_data {
    char *name;
    char modtime_s[TIME_CHAR_BUF_SIZE + 1] = {0};
    char mode[MAX_NAME_LEN + 1] = {0};
    struct stat stat;
    mode_t file_type;
} file_data_t;

typedef std::map<gid_t,std::string> gid_map_t;
typedef std::map<uid_t,std::string> uid_map_t;
typedef std::function<bool (file_data_t, file_data_t)> comparator_t;


static size_t longest_name = 0;
static size_t longest_owner = 0;
static size_t longest_group = 0;
static size_t longest_size = 0;


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

static std::string get_ansi_color(file_data_t *f)
{
    switch (f->file_type)
    {
        case S_IFDIR:
            return ANSI_BLUE;
        case S_IFLNK:
            return ANSI_LIGHT_CYAN;
        default:
            break;
    }
    // TODO: better rule for executable? Executable for me only?
    if ((f->stat.st_mode & S_IXUSR) == S_IXUSR)
    {
        return ANSI_LIGHT_GREEN;
    }
    return ANSI_NOTHING;
}

static comparator_t choose_comparator(sort_type_t sort_type)
{
    switch (sort_type)
    {
        case MODTIME:
            return [](file_data_t a, file_data_t b)
            {
                return a.stat.st_mtim.tv_sec < b.stat.st_mtim.tv_sec;
            };
        case NAME_REVERSE:
            return [](file_data_t a, file_data_t b)
            {
                return strcasecmp(a.name,b.name) > 0;
            };
        case MODTIME_REVERSE:
            return [](file_data_t a, file_data_t b)
            {
                return a.stat.st_mtim.tv_sec > b.stat.st_mtim.tv_sec;
            };
        default:
            return [](file_data_t a, file_data_t b)
            {
                return strcasecmp(a.name,b.name) < 0;
            };
    }
}

int print_dir_listing(const char * dirname, bool long_list, bool list_all,
                      sort_type_t sort_type)
{
    int ret;
    DIR * dirp;
    struct dirent * dp;

    if ((dirp = opendir(dirname)) == NULL)
    {
        fprintf(stderr,"cannot access %s: %s\n",dirname,strerror(errno));
        return LISTER_ERROR;
    }

    std::string dirstring = dirname;
    // use these for caching to reduce syscalls
    gid_map_t gid_map;
    uid_map_t uid_map;

    // list all of the files in the directory
    std::vector<file_data_t> files;
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

        file_data_t f;
        if ((ret = lstat(fullpath.c_str(),&f.stat)) != 0)
        {
            // report the error somehow
            fprintf(stderr,"unable to stat %s: %s\n",
                    fullpath.c_str(),strerror(errno));
        }

        f.name = dp->d_name;
        size_t len = strlen(f.name);
        if (len > longest_name)
        {
            longest_name = len;
        }
        // modification time, which is what ls uses for sorting
        f.file_type = f.stat.st_mode & S_IFMT;
        files.push_back(f);
    }

    std::sort( files.begin(), files.end(), choose_comparator(sort_type) );

    char format_string[MAX_LINE_LEN + 1] = {0};
    if (long_list)
    {
        size_t len;
        time_t now = time(NULL);
        struct group *grpbuf;
        struct passwd *nambuf;
        std::string time_format;

        for (auto &f : files)
        {
            // cache the group name to avoid needless syscalls
            if (gid_map.find(f.stat.st_gid) == gid_map.end())
            {
                grpbuf = getgrgid(f.stat.st_gid);
                gid_map[f.stat.st_gid] = grpbuf->gr_name;
                len = strlen(grpbuf->gr_name);
                if (len > longest_group)
                {
                    longest_group = len;
                }
            }

            // cache the user name to avoid needless syscalls
            if (uid_map.find(f.stat.st_uid) == uid_map.end())
            {
                nambuf = getpwuid(f.stat.st_uid);
                uid_map[f.stat.st_uid] = nambuf->pw_name;
                len = strlen(nambuf->pw_name);
                if (len > longest_owner)
                {
                    longest_owner = len;
                }
            }

            // we also want the string representation, for when we print
            if (now - f.stat.st_mtim.tv_sec > SIX_MONTHS_SECS)
            {
                time_format = "%b %e  %Y";
            }
            else
            {
                time_format = "%b %e %H:%M";
            }
            strftime(f.modtime_s,TIME_CHAR_BUF_SIZE,time_format.c_str(),localtime(&f.stat.st_mtim.tv_sec));
            strcpy(f.mode,(char *)mode_to_string(f.stat.st_mode).c_str());

            if (f.stat.st_size > 0)
            {
                len = (int) log10(f.stat.st_size) + 1;
                if (len > longest_size)
                {
                    longest_size = len;
                }
            }
        }

        sprintf(format_string,"%%s %%%zds %%%zds %%%zdzd %%s %%s%%-%zds\x1b[0;0m",
                longest_owner,longest_group,longest_size,longest_name);
    }
    else
    {
        sprintf(format_string,"%%s%%-%zds\x1b[0;0m",longest_name);
    }

    for (auto &f : files)
    {
        if (long_list)
        {
            printf(format_string,f.mode,uid_map[f.stat.st_uid].c_str(),
                   gid_map[f.stat.st_gid].c_str(),f.stat.st_size,
                   f.modtime_s,get_ansi_color(&f).c_str(),f.name);
        }
        else
        {
            printf(format_string,get_ansi_color(&f).c_str(),f.name);
        }
        printf(" comment");
        printf("\n");
    }

    closedir(dirp);
    return 0;
}
