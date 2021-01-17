/*
 * Copyright (c) 2019 Opticks Team. All Rights Reserved.
 *
 * This file is part of Opticks
 * (see https://bitbucket.org/simoncblyth/opticks).
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); 
 * you may not use this file except in compliance with the License.  
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software 
 * distributed under the License is distributed on an "AS IS" BASIS, 
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  
 * See the License for the specific language governing permissions and 
 * limitations under the License.
 */

#include "OPTICKS_LOG.hh"
#include "SGDML.hh"


struct Demo
{
   int answer ;
};


void test_GenerateName()
{
    Demo* d = new Demo { 42 } ; 
    LOG(info) << SGDML::GenerateName( "Demo", d, true );
}


void test_Strip()
{
    std::string name = "hello0xworld0xcruel" ; 
    name = SGDML::Strip(name) ; 
    LOG(info) << name ; 
}



int main(int argc, char** argv)
{   
    OPTICKS_LOG(argc, argv);

    test_GenerateName(); 
    test_Strip(); 
 
    return 0 ;
}   

