#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <boost/program_options.hpp>
#include <lister.hpp>
#include <comment.hpp>


typedef void (*opt_handler)(std::string, std::vector<std::string>, bool);


static bool user_override(void)
{
    std::string input;
    std::cout << "Overwrite existing comment? [Y/n]: ";
    std::getline(std::cin,input);
    if (input[0] == 'Y')
        return true;

    return false;
}

static void add_cmnt(std::string filename, std::vector<std::string> opts,
                     bool help)
{
    namespace po = boost::program_options;
    po::options_description args("Adder arguments");
    args.add_options()
        ("comment,c", po::value<std::vector<std::string>>(), "comment text")
        ("overwrite,o", "overwrite comment if one already exists")
        ("force,f", "overwrite existing comment without warning")
        ("help,h", "print this message and exit")
    ;
    if (help)
    {
        std::cout << "USAGE: cmnt add [path]\n";
        std::cout << args << std::endl;
        std::exit(1);
    }

    bool overwrite = false;
    bool force = false;
    try
    {
        po::variables_map vm;
        po::store(po::command_line_parser(opts)
                    .options(args).run(), vm);
        po::notify(vm);

        if (vm.count("overwrite"))
        {
            overwrite = true;
        }
        if (vm.count("force"))
        {
            force = true;
        }
    }
    catch ( const boost::program_options::error &e )
    {
        std::cerr << e.what() << std::endl;
        std::exit(1);
    }

    if (has_comment(filename))
    {
        if (not overwrite)
        {
            fprintf(stderr,"comment already exists\n");
            return;
        }
        else if (not force and not user_override())
        {
            fprintf(stderr,"comment not written\n");
            return;
        }
    }
}

static void update_cmnt(std::string filename, std::vector<std::string> opts,
                        bool help)
{

}

static void remove_cmnt(std::string filename, std::vector<std::string> opts,
                        bool help)
{

}

static void display_cmnt(std::string filename, std::vector<std::string> opts,
                         bool help)
{

}

static void list_cmnts(std::string filename, std::vector<std::string> opts,
                       bool help)
{
    namespace po = boost::program_options;
    po::options_description args("Lister arguments");
    args.add_options()
        ("long,l", "print long listings for files")
        ("all,a", "list all files (including dot files)")
        ("reverse,r", "sort listings in reverse order")
        ("time,t", "sort by last modification time")
        ("help,h", "print this message and exit")
    ;
    if (help)
    {
        std::cout << "USAGE: cmnt list [path]\n";
        std::cout << args << std::endl;
        std::exit(1);
    }

    bool list_all = false;
    bool long_listing = false;
    sort_type_t sort_type = DEFAULT_SORT;
    try
    {
        po::variables_map vm;
        po::store(po::command_line_parser(opts)
                    .options(args).run(), vm);
        po::notify(vm);

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
    print_dir_listing(filename.c_str(),long_listing,list_all,sort_type);
}

static void help_and_exit(std::string filename, std::vector<std::string> opts,
                          bool help=true)
{
    std::cout << "help!\n";
}

static std::map<std::string, opt_handler> commands
{
    std::make_pair ("add",      add_cmnt),
    std::make_pair ("update",   update_cmnt),
    std::make_pair ("remove",   remove_cmnt),
    std::make_pair ("display",  display_cmnt),
    std::make_pair ("list",     list_cmnts),
    std::make_pair ("help",     help_and_exit),
};

int main(int argc, char ** argv)
{
    namespace po = boost::program_options;
    po::options_description main_arg;
    main_arg.add_options()
        ("arg",         po::value<std::string>(),"")
        ("filename",    po::value<std::string>(),"")
        ("help,h",      "show this message and exit")
    ;
    po::positional_options_description main_pos;
    main_pos.add("arg",1).add("filename",-1);
    po::parsed_options parsed = po::command_line_parser(argc,argv).
        options(main_arg).
        positional(main_pos).
        allow_unregistered().
        run();

    bool help = false;
    std::string cmd = "";
    std::string filename = "";
    std::vector<std::string> opts;
    try
    {
        po::variables_map vm;
        po::store(parsed, vm);
        if (vm.count("arg"))
        {
            cmd = vm["arg"].as<std::string>();
        }
        if (vm.count("filename"))
        {
            filename = vm["filename"].as<std::string>();
        }
        if (vm.count("help"))
        {
            help = true;
        }
    }
    catch ( const po::error &e )
    {
        std::cerr << e.what() << std::endl;
        std::exit(1);
    }

    opts = po::collect_unrecognized(parsed.options,po::include_positional);
    if (commands.find(cmd) != commands.end())
    {
        if (!(help or cmd == "help") and filename == "")
        {
            std::cerr << "error: filename expected\n";
            std::cerr << "use cmnt " << cmd << " --help for more information\n";
            std::exit(1);
        }
        opts.erase(opts.begin());
        commands[cmd](filename,opts,help);
    }
    else if (help)
    {
        help_and_exit(filename,opts);
    }
    else
    {
        std::string path = ".";
        if (cmd != "")
        {
            path = cmd;
        }
        list_cmnts(path,opts,false);
    }
}
