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


static std::string get_pager(void)
{
    char * pager_str = getenv("PAGER");
    if (pager_str != nullptr)
    {
        return std::string(pager_str);
    }

    return DEFAULT_PAGER;
}


static std::string create_temp_file(std::string comment="\n")
{
    pid_t pid = getpid();
    std::string fname = "/tmp/cmnt-tmp." + std::to_string(pid) + ".txt";
    std::string prompt = comment + "\n" + comment_prompt;

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

    size_t line_count = 0;
    std::string line;
    while (std::getline(temp, line))
    {
        if (line == "") continue;
        if (line[0] == '#') continue;

        if (line_count > 0)
        {
            comment += "\n";
        }
        comment += line;
        line_count++;
    }

    temp.close();

    return comment;
}


static void remove_file(std::string filename)
{
    if (std::remove(filename.c_str()) != 0)
    {
        fprintf(stderr,"error deleting file %s\n",filename.c_str());
        std::exit(1);
    }
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

    remove_file(temp);
    return comment;
}


std::string update_comment_from_file(std::string filename)
{
    std::string comment = "";

    return comment;
}


void display_comment(std::string comment)
{
    std::string pager = get_pager();
    std::string temp = create_temp_file(comment);

    std::string cmd = pager + " " + temp;
    if (system(cmd.c_str()) < 0)
    {
        fprintf(stderr,"unable to use %s pager: "
                "make sure $PAGER is set properly",pager.c_str());
        std::exit(1);
    }

    remove_file(temp);
}
