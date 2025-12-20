#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <sstream>
#include <unistd.h>

#define nl std::endl
std::vector<std::string> builtin={
  std::string("exit"),
  std::string("echo"),
  std::string("type")
};  
//I am defining type as a function because its code can be reused to search for
//executables in PATH
std::string type(std::string command){
  for(auto i:builtin)
    if(i==command){
      return std::string(" is a shell builtin");
      break;
    }

  std::string path=getenv("PATH");
  std::stringstream ss(path);
  std::string tocheck;
  while(std::getline(ss,tocheck,':')){
    tocheck=tocheck+'/'+command;
    if(access(tocheck.c_str(),X_OK)==0){
      return std::string(" is "+tocheck);
    }
  }

  return std::string(": not found");
}

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  std::string in;

  while(true){
    std::cout << "$ ";
    getline(std::cin,in,'\n');

    std::string command; //stores the command, will check this to perform operations
    for(char c:in){
      if(c==' '||c=='\n') break;
      command+=c;
    }

    if(command.empty())
      continue;

    //exit
    else if(command==builtin[0])
      goto exit;
    
    //echo
    else if(command==builtin[1])
      std::cout<<in.substr(5)<<nl;
    
    //type
    else if(command==builtin[2]){
      std::string check=in.substr(5);
      std::cout<<check<<type(check)<<nl;
    }

    //must be last if statement so if the command was builtin that if matches first
    else if(type(command)!=": not found"){ //if it was a shell buitlin we would have encountered it before
     // std::string exec=(type(command)).substr(4); //path to executable
    //  std::string args=in.substr(command.length());
      system(in.c_str());
    }
    //if none match
    else
      std::cout<<in<<": command not found"<<nl;
      
  }
  exit:
  return 0;
}
