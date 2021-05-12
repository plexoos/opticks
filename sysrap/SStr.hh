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

#pragma once

/**
SStr
======

Static string utilities.



**/


#include <cstddef>
#include <string>
#include <vector>
#include <array>
#include <glm/fwd.hpp>


#include "SYSRAP_API_EXPORT.hh"

class SYSRAP_API SStr {

    typedef unsigned long long ULL ;
  public:
      static void Save(const char* path, const std::vector<std::string>& a, char delim='\n' );    

      static void FillFromULL( char* dest, unsigned long long value, char unprintable='.') ; 
      static const char* FromULL(unsigned long long value, char unprintable='.'); 
      static unsigned long long ToULL(const char* s8 ); 


      template <size_t SIZE>
      static const char* Format1( const char* fmt, const char* value );

      template <size_t SIZE>
      static const char* Format2( const char* fmt, const char* value1, const char* value2 );

      template <size_t SIZE>
      static const char* Format3( const char* fmt, const char* value1, const char* value2, const char* value3 );

      static bool Contains(const char* s, const char* q ); 
      static bool EndsWith( const char* s, const char* q);
      static bool StartsWith( const char* s, const char* q);

      static bool HasPointerSuffix( const char* name, unsigned hexdigits ) ;   // 12 typically, 9 with Geant4 ???
      static bool HasPointerSuffix( const char* name, unsigned min_hexdigits, unsigned max_hexdigits ) ;
      static int  GetPointerSuffixDigits( const char* name );

      static const char* Concat( const char* a, const char* b, const char* c=NULL  );
      static const char* Concat( const char* a, unsigned b   , const char* c=NULL  );
      static const char* Concat( const char* a, unsigned b, const char* c, unsigned d, const char* e  ) ; 

      template <typename T>
      static const char* Concat_( const char* a, T b   , const char* c  );


      static const char* Replace( const char* s,  char a, char b ); 
      static const char* ReplaceEnd( const char* s, const char* q, const char* r  ); 

      static void Split( const char* str, char delim,   std::vector<std::string>& elem ) ;

      static void ParseGridSpec(  std::array<int,9>& grid, const char* spec); 
      static void DumpGrid(      const std::array<int,9>& grid ) ;


      static void GetEVec(glm::vec3& v, const char* key, const char* fallback );
      static void GetEVec(glm::vec4& v, const char* key, const char* fallback );
    
      template <typename T>
      static void GetEVector(std::vector<T>& vec, const char* key, const char* fallback );

      template <typename T>
      static std::string Present(std::vector<T>& vec);


      static const char* PTXPath( const char* install_prefix, const char* cmake_target, const char* cu_stem, const char* cu_ext=".cu" );

      static void GridMinMax( const std::array<int,9>& grid, int& mn, int& mx ) ;
      static void GridMinMax( const std::array<int,9>& grid, glm::ivec3& mn, glm::ivec3& mx) ;

      static unsigned Encode4(const char* s); 

      template <typename T>
      static T ato_( const char* a );
 
      template <typename T>
      static T GetEValue(const char* key, T fallback);  


      static int AsInt(const char* arg, int fallback=-1 ) ; 
      static int ExtractInt(const char* arg, int start, unsigned num, int fallback=-1) ;


};



