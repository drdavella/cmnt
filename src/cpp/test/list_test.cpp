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
    print_dir_listing(path.c_str(),false,true);
}
