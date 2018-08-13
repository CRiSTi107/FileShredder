#include <iostream>
#include <fstream>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <cstring>
#include <chrono>
#include <sys/stat.h>

using namespace std;

#define VERSION "1.1"

#define ZERO '\0'

#define INFO_CL "-i"
#define INFO_CS "--info"

#define HELP_CS "-h"
#define HELP_CL "--help"

#define WRITE_CL "--write-file"
#define WRITE_CS "-w"

#define SIZE_CL "--size"
#define SIZE_CS "-s"

uint64_t getfilesize(const char* filename);
void writefile(const char *filename, uint64_t filesize);
int deletefile(const char *filename);
bool existfile(const char *filename);
bool isempty(const char *arg);
uint64_t chartoint64(const char *num);
bool checknum(const char *arr_num, uint64_t num);

int main(int argc, char* argv[])
{
    bool info  = false;
    bool help  = false;
    bool write = false;
    bool size  = false;

    uint64_t size_file = -1;

    //Check for arguments
    for(int index = 1; index <= argc - 1 && argv[index] != NULL; index++)
        if(!strcmp(argv[index], INFO_CL) || !strcmp(argv[index], INFO_CS))
        { info = true; argv[index] = NULL; continue; }
        else
        if(!strcmp(argv[index], HELP_CL) || !strcmp(argv[index], HELP_CS))
        { help = true; argv[index] = NULL; continue; }
        else
        if(!strcmp(argv[index], WRITE_CL) || !strcmp(argv[index], WRITE_CS))
        { write = true; argv[index] = NULL; continue; }
        else
        if(!strcmp(argv[index], SIZE_CL) || !strcmp(argv[index], SIZE_CS))
        {
            size = true;
            //get size
            //chech if it's last arg
            if(index == argc - 1)
            {
                cout << "size not specified.";
                return -1;
            }

            size_file = chartoint64(argv[index + 1]);
            argv[index] = NULL; argv[index + 1] = NULL; continue;
        }


    if(help)
    {
        string str_help(
                 string("FileShredder - VERSION ") + string(VERSION) + string(" by CRiSTi\n") +
                 string("usage: [options] [filename1] [filename2] ...\n") +
                 string("where options are:\n") +
                 string("--help,       -h   display help\n") +
                 string("--info,       -i   display info about shredded file\n") +
                 string("--write-file, -w   enable write file mode\n") +
                 string("--size,       -s   set size in bytes of written file\n")
                        );
        cout << str_help;
        return 0;
    }

    if(write && !size)
    {
        cout << "write mode requires size parameter.";
        return -1;
    }
    else if(!write && size)
    {
        cout << "enable write mode to use size";
        return -1;
    }

    for(int index = 1; index <= argc - 1; index++)
        if(argv[index] != NULL)
            if(!isempty(argv[index]))
            {
                std::string filepath = argv[index];
                uint64_t filesize = getfilesize(filepath.c_str());

                auto start = std::chrono::system_clock::now();

                if(write)
                {
                    writefile(filepath.c_str(), size_file);
                }
                else
                if(existfile(filepath.c_str()))
                {
                    writefile(filepath.c_str(), filesize);
                }
                else
                    filesize = 0;

                auto finish = std::chrono::system_clock::now();

                std::chrono::duration<double> diff = finish-start;
                if(info)
                    cout << "[" << diff.count() << "sec]" << filepath << " - " << ((write) ? size_file : filesize) << " bytes" << endl;

                if(!write)
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
    if(!existfile(filename))
        return -1;
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

uint64_t chartoint64(const char *num)
{
    uint64_t number = 0;
    for(uint64_t idx = 0; *(num + idx) != NULL; ++idx)
    {
        number *= 10;
        number += (*(num + idx) - '0');
    }
    return number;
}

uint64_t getnumbersize(uint64_t num)
{
    uint64_t num_size = 0;
    while(num > 0)
    {
        num /= 10;
        num_size++;
    }
    return num_size;
}

bool checknum(const char* arr_num, uint64_t num)
{
    uint64_t scale = 10;
    uint64_t numsize = getnumbersize(num);
    for(uint64_t idx = 0; idx <= numsize - 1; ++idx)
    {
        if((*(arr_num + idx) - '0') != (num/scale)%10)
            return false;
        scale *= 10;
    }
    return true;
}
