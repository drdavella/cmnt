#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <boost/program_options.hpp>
#include <lister.hpp>
#include <comment.hpp>
#include <comment_editor.hpp>

struct command_args
{
    std::string filename;
    std::vector<std::string> opts;
    std::string comment;
    bool needs_comment = true;
    bool help = false;
};
typedef void (*opt_handler)(struct command_args);


static bool user_override(void)
{
    std::string input;
    std::cout << "Overwrite existing comment? [Y/n]: ";
    std::getline(std::cin,input);
    if (input[0] == 'Y')
        return true;

    return false;
}

static void add_cmnt(struct command_args cargs)
{
    namespace po = boost::program_options;
    po::options_description args("Adder arguments");
    args.add_options()
        ("comment,c", po::value<std::string>(), "comment text")
        ("overwrite,o", "overwrite comment if one already exists")
        ("force,f", "overwrite existing comment without warning")
        ("help,h", "print this message and exit")
    ;
    if (cargs.help)
    {
        std::cout << "USAGE: cmnt add [path]\n";
        std::cout << args << std::endl;
        std::exit(1);
    }

    std::string comment = "";
    bool overwrite = false;
    bool force = false;
    try
    {
        po::variables_map vm;
        po::store(po::command_line_parser(cargs.opts)
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
        if (vm.count("comment"))
        {
            comment = vm["comment"].as<std::string>();
        }
    }
    catch ( const boost::program_options::error &e )
    {
        std::cerr << e.what() << std::endl;
        std::exit(1);
    }

    if (has_comment(cargs.filename))
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

    if (cargs.needs_comment)
    {
        cargs.comment = new_comment_from_file(cargs.filename);
        if (cargs.comment == "")
        {
            fprintf(stdout,"No comment added due to empty comment message\n");
            std::exit(0);
        }
    }
    add_comment(cargs.filename,cargs.comment,overwrite);
}

static void update_cmnt(struct command_args cargs)
{

}

static void remove_cmnt(struct command_args cargs)
{

}

static void display_cmnt(struct command_args cargs)
{

}

static void list_cmnts(struct command_args cargs)
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
    if (cargs.help)
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
        po::store(po::command_line_parser(cargs.opts)
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
    print_dir_listing(cargs.filename.c_str(),long_listing,list_all,sort_type);
}

static void help_and_exit(struct command_args cargs)
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
        ("comment,c",   po::value<std::string>(),"")
        ("help,h",      "show this message and exit")
    ;
    po::positional_options_description main_pos;
    main_pos.add("arg",1).add("filename",-1);
    po::parsed_options parsed = po::command_line_parser(argc,argv).
        options(main_arg).
        positional(main_pos).
        allow_unregistered().
        run();

    std::string cmd = "";
    struct command_args cargs;
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
            cargs.filename = vm["filename"].as<std::string>();
        }
        if (vm.count("comment"))
        {
            cargs.needs_comment = false;
            cargs.comment = vm["comment"].as<std::string>();
        }
        if (vm.count("help"))
        {
            cargs.help = true;
        }
    }
    catch ( const po::error &e )
    {
        std::cerr << e.what() << std::endl;
        std::exit(1);
    }

    cargs.opts = po::collect_unrecognized(parsed.options,po::include_positional);
    if (commands.find(cmd) != commands.end())
    {
        if (!(cargs.help or cmd == "help") and cargs.filename == "")
        {
            std::cerr << "error: filename expected\n";
            std::cerr << "use cmnt " << cmd << " --help for more information\n";
            std::exit(1);
        }
        cargs.opts.erase(cargs.opts.begin());
        commands[cmd](cargs);
    }
    else if (cargs.help)
    {
        help_and_exit(cargs);
    }
    else
    {
        cargs.filename = ".";
        if (cmd != "")
        {
            cargs.filename = cmd;
        }
        cargs.help = false;
        list_cmnts(cargs);
    }
}
