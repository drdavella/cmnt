#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <comment_editor.hpp>

#define MAX_NAME_LEN        32      // based on linux max username
#define TIME_CHAR_BUF_SIZE  120     // completely arbitrary
#define DEFAULT_EDITOR      "vi"
#define DEFAULT_PAGER       "less"

static std::string comment_prompt =
"# Please enter/modify the comment you wish to leave for this file.\n"
"# Lines starting with '#' will be ignored, and an empty message\n"
"# means no comment will be added.\n#\n";


static std::string get_editor(void)
{
    char * editor_str = getenv("EDITOR");
    if (editor_str != nullptr)
    {
        return std::string(editor_str);
    }

    return DEFAULT_EDITOR;
}


static std::string create_temp_file(void)
{
    pid_t pid = getpid();
    std::string fname = "/tmp/cmnt-tmp." + std::to_string(pid) + ".txt";
    std::string prompt = "\n\n" + comment_prompt;

    std::ofstream temp;
    temp.open(fname);
    if (not temp.is_open())
    {
        fprintf(stderr,"error opening temp file for writing\n");
        std::exit(1);
    }
    temp << prompt;
    temp.close();

    return fname;
}


static std::string parse_comment_file(std::string filename)
{
    std::string comment = "";

    std::ifstream temp;
    temp.open(filename);
    if (not temp.is_open())
    {
        fprintf(stderr,"error opening temp file for parsing\n");
        std::exit(1);
    }

    std::string line;
    while (std::getline(temp, line))
    {
        if (line == "") continue;
        if (line[0] == '#') continue;

        comment += line;
    }

    temp.close();

    return comment;
}


std::string new_comment_from_file(std::string filename)
{
    std::string editor = get_editor();
    std::string temp = create_temp_file();

    std::string cmd = editor + " " + temp;
    if (system(cmd.c_str()) < 0)
    {
        fprintf(stderr,"unable to use %s editor: "
                "make sure $EDITOR is set properly",editor.c_str());
        std::exit(1);
    }

    std::string comment = parse_comment_file(temp);

    // remove comment file
    if (std::remove(temp.c_str()) != 0)
    {
        fprintf(stderr,"error deleting temp file %s\n",temp.c_str());
        std::exit(1);
    }

    return comment;
}


std::string update_comment_from_file(std::string filename)
{
    std::string comment = "";

    return comment;
}
