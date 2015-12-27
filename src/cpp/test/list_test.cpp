#include <cstdlib>
#include <iostream>
#include <string>
#include <lister.hpp>
#include <boost/program_options.hpp>


int main(int argc, char ** argv)
{
    std::string path = ".";
    bool list_all = false;
    bool long_listing = false;
    sort_type_t sort_type = DEFAULT_SORT;


    namespace po = boost::program_options;
    po::options_description opt_args("Optional arguments");
    opt_args.add_options()
        ("long,l", "print long listings for files")
        ("all,a", "list all files (including dot files)")
        ("reverse,r", "sort listings in reverse order")
        ("time,t", "sort by last modification time")
        ("help,h", "print this message and exit")
    ;
    po::options_description hidden_args;
    hidden_args.add_options()
        ("path", po::value<std::string>(), "");
    po::positional_options_description po_desc;
    po_desc.add("path", 1);

    try
    {
        po::variables_map vm;
        po::options_description all_args;
        all_args.add(opt_args).add(hidden_args);
        po::store(po::command_line_parser(argc,argv).
                  options(all_args).positional(po_desc).run(), vm);
        po::notify(vm);

        if (vm.count("help"))
        {
            std::cout << "USAGE: list_test [path]\n";
            std::cout << opt_args << std::endl;
            std::exit(1);
        }
        if (vm.count("path"))
        {
            path = vm["path"].as<std::string>();
        }
        if (vm.count("long"))
        {
            long_listing = true;
        }
        if (vm.count("all"))
        {
            list_all = true;
        }
        if (vm.count("reverse") && vm.count("time"))
        {
            sort_type = MODTIME_REVERSE;
        }
        else if (vm.count("time"))
        {
            sort_type = MODTIME;
        }
        else if (vm.count("reverse"))
        {
            sort_type = NAME_REVERSE;
        }
    }
    catch ( const boost::program_options::error &e )
    {
        std::cerr << e.what() << std::endl;
        std::exit(1);
    }

    print_dir_listing(path.c_str(),long_listing,list_all,sort_type);
}
