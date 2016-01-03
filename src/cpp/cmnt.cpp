#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <boost/program_options.hpp>
#include <lister.hpp>
#include <comment.hpp>


typedef void (*opt_handler)(std::vector<std::string> s);

static void add_cmnt(std::vector<std::string> opts)
{

}

static void update_cmnt(std::vector<std::string> opts)
{

}

static void remove_cmnt(std::vector<std::string> opts)
{

}

static void display_cmnt(std::vector<std::string> opts)
{

}

static void list_cmnts(std::vector<std::string> opts)
{

}

static void help_and_exit(std::vector<std::string> opts)
{

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
        ("arg",     po::value<std::string>(),"")
        ("subargs", po::value<std::string>(),"")
    ;
    po::positional_options_description main_pos;
    main_pos.add("arg",1).add("subargs",-1);
    po::parsed_options parsed = po::command_line_parser(argc,argv).
        options(main_arg).
        positional(main_pos).
        allow_unregistered().
        run();

    std::string cmd = "";
    try
    {
        po::variables_map vm;
        po::store(parsed, vm);
        if (vm.count("arg"))
        {
            cmd = vm["arg"].as<std::string>();
        }
    }
    catch ( const po::error &e )
    {
        std::cerr << e.what() << std::endl;
        std::exit(1);
    }

    if (commands.find(cmd) != commands.end())
    {
        std::vector<std::string> opts =
            po::collect_unrecognized(parsed.options,po::include_positional);
        opts.erase(opts.begin());
        commands[cmd](opts);
    }
    else
    {
        std::string path = ".";
        if (cmd != "")
        {
            path = cmd;
        }
    }
}
