#include <iostream>
#include <fstream>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <cstring>
#include <chrono>
#include <sys/stat.h>

using namespace std;

#define VERSION "1.0"

#define ZERO '\0'

#define INFO_CL "-i"
#define INFO_CS "--info"

#define HELP_CS "-h"
#define HELP_CL "--help"


uint64_t getfilesize(const char* filename);
void writefile(const char *filename, uint64_t filesize);
int deletefile(const char *filename);
bool existfile(const char *filename);
bool isempty(const char *arg);


int main(int argc, char* argv[])
{
    bool info  = false;
    bool help  = false;

    //Check for arguments
    for(int index = 1; index <= argc && argv[index] != NULL; index++)
        if(!strcmp(argv[index], INFO_CL) || !strcmp(argv[index], INFO_CS))
        { info = true; argv[index] = NULL; continue; }
        else
        if(!strcmp(argv[index], HELP_CL) || !strcmp(argv[index], HELP_CS))
        { help = true; argv[index] = NULL; continue; }


    if(help)
    {
        string str_help(
                 string("usage: [options] [filename1] [filename2] ...\n") +
                 string("where options are:\n") +
                 string("--help, -h  display help\n") +
                 string("--info, -i  display info about shredded file\n")
                        );
        cout << str_help;
        return 0;
    }


    for(int index = 1; index <= argc; index++)
        if(argv[index] != NULL)
            if(!isempty(argv[index]))
            {
                std::string filepath = argv[index];
                uint64_t filesize = getfilesize(filepath.c_str());

                auto start = std::chrono::system_clock::now();
                if(existfile(filepath.c_str()))
                    writefile(filepath.c_str(), filesize);
                else
                    filesize = 0;
                auto finish = std::chrono::system_clock::now();

                std::chrono::duration<double> diff = finish-start;
                if(info)
                    cout << "[" << diff.count() << "sec]" << filepath << " - " << filesize << " bytes" << endl;

                deletefile(filepath.c_str());
            }

    return 0;
}

bool isempty(const char *arg)
{
    return (*arg == ZERO) ? true : false;
}

uint64_t getfilesize(const char* filename)
{
    std::ifstream in(filename, std::ios::binary | std::ios::ate);
    return (uint64_t)in.tellg();
}

void writefile(const char *filename, uint64_t filesize)
{
    char ZEROS[10240];
    memset(ZEROS, ZERO, sizeof(ZEROS));

    std::ofstream out(filename, std::ios::out);

    while(10240 < filesize)
    {
        out.write(ZEROS, 10240);
        filesize -= 10240;
    }

    out.write(ZEROS, filesize);

    out.close();
}

int deletefile(const char *filename)
{
    return remove(filename);
}

bool existfile (const char *filename)
{
  struct stat buffer;
  return (stat (filename, &buffer) == 0);
}
