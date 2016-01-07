#include <cstdio>
#include <string>
#include <boost/program_options.hpp>
#include <comment.hpp>


int main(int argc, char ** argv)
{
    std::string comment = "this is a really long comment, I hope that it exceeds the line";
    add_comment(argv[1],comment.c_str());


}
