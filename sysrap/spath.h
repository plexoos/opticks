#pragma once
/**
spath.h
=========

Q: Whats the difference between spath::ResolvePath and spath::Resolve ? 
A: ResolvePath accepts only a single string element whereas Resolve accepts
   from 1 to 4 elements

   * so incon

**/

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <iostream>

struct spath
{
    friend struct spath_test ; 
    static constexpr const bool VERBOSE = false ; 

private:
    static std::string _ResolvePath(const char* spec); 
    static const char* ResolvePath(const char* spec); 

    static char* DefaultTMP();
    static char* ResolveToken(const char* token); 
    static char* _ResolveToken(const char* token); 
    static bool  IsTokenWithFallback(const char* token); 
    static char* _ResolveTokenWithFallback(const char* token); 

    template<typename ... Args>
    static std::string _Resolve(Args ... args ); 

public:
    template<typename ... Args>
    static const char* Resolve(Args ... args ); 

private:
    template<typename ... Args>
    static std::string _Join( Args ... args_  ); 

    template<typename ... Args>
    static std::string _Name( Args ... args_  ); 

public:
    template<typename ... Args>
    static const char* Join( Args ... args ); 

    template<typename ... Args>
    static const char* Name( Args ... args ); 


    template<typename ... Args>
    static bool Exists( Args ... args ); 

    static bool LooksLikePath(const char* arg); 
    static const char* Basename(const char* path); 

    static int Remove(const char* path_); 

};


/**
spath::_ResolvePath
----------------------

This works with multiple tokens, eg::

    $HOME/.opticks/GEOM/$GEOM/CSGFoundry/meshname.txt

**/

inline std::string spath::_ResolvePath(const char* spec_)
{
    if(spec_ == nullptr) return "" ; 
    char* spec = strdup(spec_); 

    std::stringstream ss ; 
    int speclen = int(strlen(spec)) ;  
    char* end = strchr(spec, '\0' ); 
    int i = 0 ; 

    if(VERBOSE) std::cout << " spec " << spec << " speclen " << speclen << std::endl ; 

    while( i < speclen )
    {
        if(VERBOSE) std::cout << " i " << i << " spec[i] " << spec[i] << std::endl ;   
        if( spec[i] == '$' )
        {
            char* p = spec + i ; 
            char* sep = strchr( p, '/' ) ; // first slash after token   
            bool tok_plus =  sep && end && sep != end ;  
            if(tok_plus) *sep = '\0' ;           // replace slash with null termination 
            char* val = ResolveToken(p+1) ;  // skip '$'
            int toklen = int(strlen(p)) ;  // strlen("TOKEN")  no need for +1 as already at '$'  
            if(VERBOSE) std::cout << " toklen " << toklen << std::endl ;  
            if(val == nullptr) 
            {
                std::cerr 
                    << "spath::_ResolvePath token [" 
                    << p+1 
                    << "] does not resolve " 
                    << std::endl 
                    ; 
                //return "" ;    // all tokens must resolve 
                ss << "UNRESOLVED_TOKEN_" << (p+1) ; 
            }
            else
            {
                ss << val ; 
            }
            if(tok_plus) *sep = '/' ;  // put back the slash 
            i += toklen ;              // skip over the token 
        }
        else
        {
           ss << spec[i] ; 
           i += 1 ; 
        }
    }
    std::string str = ss.str(); 
    return str ; 
}

inline const char* spath::ResolvePath(const char* spec_)
{
    std::string path = _ResolvePath(spec_) ;
    return strdup(path.c_str()) ; 
}

inline char* spath::DefaultTMP()
{
    char* user = getenv("USER") ; 
    std::stringstream ss ; 
    ss << "/tmp/" << ( user ? user : "MISSING_USER" ) << "/" << "opticks" ;   
    std::string str = ss.str(); 
    return strdup(str.c_str()); 
}

inline char* spath::ResolveToken(const char* token)
{
    return IsTokenWithFallback(token) 
        ?
            _ResolveTokenWithFallback(token)
        :
            _ResolveToken(token)
        ; 
}

/**
spath::_ResolveToken
----------------------

The token "TMP" is special cased to resolve as /tmp/$USER/opticks
when no TMP envvar is defined. 

**/


inline char* spath::_ResolveToken(const char* token)
{
    char* val = getenv(token) ; 
    if( val == nullptr && strcmp(token, "TMP") == 0) val = DefaultTMP() ; 
    return val ; 
}

/**
spath::IsTokenWithFallback
---------------------------

Bash style token with fallback::

   ${U4Debug_SaveDir:-$TMP}   # original 
   {U4Debug_SaveDir:-$TMP}    # when arrives here from ResolveToken 


**/

inline bool spath::IsTokenWithFallback(const char* token)
{
    return token && strlen(token) > 0 && token[0] == '{' && token[strlen(token)-1] == '}' && strstr(token,":-$") != nullptr  ; 
}

/**
spath::_ResolveTokenWithFallback
----------------------------------

Currently only simple fallback token are supported. 

TODO: handle more general fallbacks like ${TMP:-/tmp/$USER/opticks}

**/

inline char* spath::_ResolveTokenWithFallback(const char* token_)
{
    assert( IsTokenWithFallback(token_) ); 

    char* token = strdup(token_); 
    token[strlen(token)-1] = '\0' ;  // overwrite the trailing '}' 

    const char* delim = ":-$" ; 
    char* split = strstr(token, delim) ; 

    bool dump = false ; 
 
    if(dump) std::cout 
       << "spath::ResolveTokenWithFallback" 
       << std::endl 
       << " token " << ( token ? token : "-" )
       << std::endl 
       << " split " << ( split ? split : "-" )
       << std::endl 
       ;

    assert( split );  
    char* tok1 = split + strlen(delim)  ; 

    split[0] = '\0' ; 
    char* tok0 = token + 1 ; 
    
    if(dump) std::cout 
       << "spath::ResolveTokenWithFallback" 
       << std::endl 
       << " tok0 " << ( tok0 ? tok0 : "-" )
       << std::endl 
       << " tok1 " << ( tok1 ? tok1 : "-" )
       << std::endl 
       ;

    char* val = _ResolveToken(tok0) ; 
    if(val == nullptr) val = _ResolveToken(tok1) ; 
    return val ; 
}




template<typename ... Args>
inline std::string spath::_Resolve( Args ... args  )  // static
{
    std::string spec = _Join(std::forward<Args>(args)... ); 
    return _ResolvePath(spec.c_str()); 
}

template std::string spath::_Resolve( const char* ); 
template std::string spath::_Resolve( const char*, const char* ); 
template std::string spath::_Resolve( const char*, const char*, const char* ); 
template std::string spath::_Resolve( const char*, const char*, const char*, const char* ); 


template<typename ... Args>
inline const char* spath::Resolve( Args ... args  )  // static
{
    std::string spec = _Join(std::forward<Args>(args)... ); 
    std::string path = _ResolvePath(spec.c_str()); 
    return strdup(path.c_str()) ; 
}

template const char* spath::Resolve( const char* ); 
template const char* spath::Resolve( const char*, const char* ); 
template const char* spath::Resolve( const char*, const char*, const char* ); 
template const char* spath::Resolve( const char*, const char*, const char*, const char* ); 




template<typename ... Args>
inline std::string spath::_Join( Args ... args_  )  // static
{
    std::vector<std::string> args = {args_...};
    std::vector<std::string> elem ; 

    for(unsigned i=0 ; i < args.size() ; i++)
    {
        const std::string& arg = args[i] ; 
        if(!arg.empty()) elem.push_back(arg);  
    }

    unsigned num_elem = elem.size() ; 
    std::stringstream ss ; 
    for(unsigned i=0 ; i < num_elem ; i++)
    {
        const std::string& ele = elem[i] ; 
        ss << ele << ( i < num_elem - 1 ? "/" : "" ) ; 
    }
    std::string s = ss.str(); 
    return s ; 
}   

template std::string spath::_Join( const char* ); 
template std::string spath::_Join( const char*, const char* ); 
template std::string spath::_Join( const char*, const char*, const char* ); 
template std::string spath::_Join( const char*, const char*, const char*, const char* ); 

template<typename ... Args>
inline const char* spath::Join( Args ... args )  // static
{
    std::string s = _Join(std::forward<Args>(args)...)  ; 
    return strdup(s.c_str()) ; 
}   

template const char* spath::Join( const char* ); 
template const char* spath::Join( const char*, const char* ); 
template const char* spath::Join( const char*, const char*, const char* ); 
template const char* spath::Join( const char*, const char*, const char*, const char* ); 








template<typename ... Args>
inline std::string spath::_Name( Args ... args_  )  // static
{
    std::vector<std::string> args = {args_...};
    std::vector<std::string> elem ; 

    for(unsigned i=0 ; i < args.size() ; i++)
    {
        const std::string& arg = args[i] ; 
        if(!arg.empty()) elem.push_back(arg);  
    }

    const char* delim = "" ; 

    unsigned num_elem = elem.size() ; 
    std::stringstream ss ; 
    for(unsigned i=0 ; i < num_elem ; i++)
    {
        const std::string& ele = elem[i] ; 
        ss << ele << ( i < num_elem - 1 ? delim : "" ) ; 
    }
    std::string s = ss.str(); 
    return s ; 
}   

template std::string spath::_Name( const char* ); 
template std::string spath::_Name( const char*, const char* ); 
template std::string spath::_Name( const char*, const char*, const char* ); 
template std::string spath::_Name( const char*, const char*, const char*, const char* ); 

template<typename ... Args>
inline const char* spath::Name( Args ... args )  // static
{
    std::string s = _Name(std::forward<Args>(args)...)  ; 
    return strdup(s.c_str()) ; 
}   

template const char* spath::Name( const char* ); 
template const char* spath::Name( const char*, const char* ); 
template const char* spath::Name( const char*, const char*, const char* ); 
template const char* spath::Name( const char*, const char*, const char*, const char* ); 





















template<typename ... Args>
inline bool spath::Exists(Args ... args)
{
    std::string path = _Resolve(std::forward<Args>(args)...) ; 
    std::ifstream fp(path.c_str(), std::ios::in|std::ios::binary);
    return fp.fail() ? false : true ; 
}

template bool spath::Exists( const char* ); 
template bool spath::Exists( const char*, const char* ); 
template bool spath::Exists( const char*, const char*, const char* ); 
template bool spath::Exists( const char*, const char*, const char*, const char* ); 


inline bool spath::LooksLikePath(const char* arg)
{
    if(!arg) return false ;
    if(strlen(arg) < 2) return false ; 
    return arg[0] == '/' || arg[0] == '$' ; 
}

inline const char* spath::Basename(const char* path)
{
    std::string p = path ; 
    std::size_t pos = p.find_last_of("/");
    std::string base = pos == std::string::npos ? p : p.substr(pos+1) ; 
    return strdup( base.c_str() ) ; 
}


inline int spath::Remove(const char* path_)
{
    const char* path = spath::Resolve(path_); 
    assert( strlen(path) > 2 ); 
    return remove(path);  
}



