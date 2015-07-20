#include "assert.h"
#include "stdio.h"
#include "stdlib.h"

#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>


int main(int argc, char** argv)
{
    const char* flags = "/tmp/GFlagIndexLocal.ini";
    std::ifstream fs(flags, std::ios::in);
    std::string line = ""; 

    std::vector<std::string> names ; 
    std::vector<unsigned int> vals ; 

    while(!fs.eof()) 
    {   
        std::getline(fs, line);
        
        const char* eq = strchr( line.c_str(), '=');
        if(eq)
        {
            std::string name = line.substr(0, eq - line.c_str());        
            std::string value = eq + 1 ;        
    
            names.push_back(name);
            vals.push_back(atoi(value.c_str()));
 
            std::cout 
                << std::setw(30) << name 
                << std::setw(30) << value 
                << std::endl ; 
        }
    }   


    for(unsigned int i=0 ; i<names.size() ; i++)
    {
        std::cout 
              << std::setw(30) << i
              << std::setw(30) << names[i] 
              << std::setw(30) << vals[i] 
              << std::endl ; 
    }

    return 0 ;
}



