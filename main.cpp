#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>
#include <map>

#define nl std::endl
using func = int (*)(std::vector<std::string>);

bool flag=true;
std::map<std::string,func> builtin;
std::vector<std::string> history_data;

typedef struct {
    std::string command;
    std::vector<std::string> args;
} input;

void parse_input(input* in,std::string data) {
    std::stringstream ss(data);
    ss >> in->command;
    std::string str;
    while (ss >> str)
        in->args.push_back(str);

}

int check_File_executable(std::string name,std::string *dir){
    std::string path = getenv("PATH");
    path=path+":./";
    std::stringstream ss(path);
    std::string tocheck;
    while (std::getline(ss, tocheck, ':')) {
        tocheck = tocheck + '/' + name;
        if (access(tocheck.c_str(), X_OK) == 0) {
            (*dir)=tocheck;
            return 0;
        }
    }
    return -1;
}

int builtin_echo(std::vector<std::string> args) {
    for (std::string i : args)
        std::cout << i << " ";
    std::cout<<nl;
    return 0;
}

int builtin_exit(std::vector<std::string> args){
  flag=false;
    std::cout<<nl;
  return 0;
}

// I am defining type as a function because its code can be reused to search for
// executables in PATH
int builtin_type(std::vector<std::string> args) {
    if(args.empty()){
        std::cerr<<"Error: No arguments"<<nl;
        return -1;
    }
    std::cout<<args[0];
    std::string dir;
    if (builtin.count(args[0])!=0)
        std::cout<<" is a shell builtin"<<nl;
    else if(check_File_executable(args[0],&dir)!=-1)
        std::cout<<" is "<<dir<<nl;
    else
        std::cout<<": not found"<<nl;
    return 1;
}

int builtin_history(std::vector<std::string> args){
    int count=1;
    int len=history_data.size();
    if (args.size() >= 1) {
        try {
            int k;
            std::size_t pos;
            k=std::stoi(args[0], &pos);
            if (pos == args[0].length()){
                if(k>=0)
                    count=std::max(len-k+1,1); //last k elements
                else{
                    len=-k; // first k elements
                }
            }
            else{
                std::cout<<"Not a Number"<<nl;
                return -1; 
            }
        }
        catch (std::invalid_argument const &ex) {
            std::cout<<"Not a Number"<<nl;
            return -1;
        }
    }
    for(;count<=len;count++){
        std::cout<<count<<" "<<history_data[count-1]<<nl;
    }
    return 0;
}

void init(){
    builtin["exit"]=&builtin_exit;
    builtin["echo"]=&builtin_echo;
    builtin["type"]=&builtin_type;
    builtin["history"]=&builtin_history;
}

int main() {
    // Flush after every std::cout / std:cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    std::string data;
    std::string dir;

    init();

    while (flag) {
        std::cout << "$ ";
        getline(std::cin, data, '\n');
        history_data.push_back(data);
        input *in = new input;
        if(in==NULL){
          std::cerr<<"Error allocating";
          return -1;
        }
        parse_input(in,data);
        if (in->command.empty())
            std::cout<<nl;
        else if(builtin.count(in->command)!=0){
            builtin[in->command](in->args);
        }
        else if (check_File_executable(in->command,&dir)!=-1) {
            std::string executable= in->command;
            for(std::string i:in->args)
                executable+=" "+i;
            system(executable.c_str());
        }
        // if none match
        else
            std::cout << in->command << ": command not found"<<nl;
        //delete section
        delete in;
        
    }
    return 0;
}
