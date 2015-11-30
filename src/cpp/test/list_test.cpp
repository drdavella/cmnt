#include <cstdlib>
#include <iostream>
#include <string>
#include <cmnt_lister.hpp>


int main(int argc, char ** argv)
{
    std::string path;
    if (argc == 2)
    {
        path = std::string(argv[1]);
    }
    else
    {
        path = ".";
    }
    FileDataList * list = get_dir_listing(path.c_str(),true,true);
    if (list == nullptr)
    {
        std::cerr << "cannot access " << path << ": no such file or directory\n";
        std::exit(1);
    }
    list->sort_by(DEFAULT_SORT);
    list->list_data(std::cout,0,true);
}
