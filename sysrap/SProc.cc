#include "SProc.hh"
#include "PLOG.hh"


#ifdef _MSC_VER
float SProc::VirtualMemoryUsageMB()
{
    return 0 ; 
}

#elif defined(__APPLE__)

#include<mach/mach.h>

float SProc::VirtualMemoryUsageMB()
{
    struct mach_task_basic_info info;
    mach_msg_type_number_t size = MACH_TASK_BASIC_INFO_COUNT;
    kern_return_t kerr = task_info(mach_task_self(),
                                   MACH_TASK_BASIC_INFO,
                                   (task_info_t)&info,
                                   &size);

    if( kerr == KERN_SUCCESS ) 
    {
        vm_size_t vsize_ = info.virtual_size  ;  
        unsigned long long vsize(vsize_); 
        unsigned long long MB = 1000000 ; 
        float usage = float(vsize/MB) ; 
        return usage  ;
    }

    LOG(error) << mach_error_string(kerr)   ; 

    return 0 ;   
}


#else
    
// https://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process

#include "stdlib.h"
#include "stdio.h"
#include "string.h"

int parseLine(char* line){
    // This assumes that a digit will be found and the line ends in " Kb".
    int i = strlen(line);
    const char* p = line;
    while (*p <'0' || *p > '9') p++;
    line[i-3] = '\0';
    i = atoi(p);
    return i;
}

float SProc::VirtualMemoryUsageMB()
{
    FILE* file = fopen("/proc/self/status", "r");
    float result = 0.f ;
    char line[128];

    while (fgets(line, 128, file) != NULL){
        if (strncmp(line, "VmSize:", 7) == 0){
            result = parseLine(line);   // value in Kb 
            result /= 1000.f ;          // convert to MB
            break;
        }
    }
    fclose(file);
    return result;
}
#endif





#ifdef _MSC_VER
const char* SProc::ExecutablePath()
{
    return NULL ; 
}
#elif defined(__APPLE__)
const char* SProc::ExecutablePath()
{
    return NULL ; 
}
#else


#include <unistd.h>
#include <limits.h>

const char* SProc::ExecutablePath(bool basename)
{
    char buf[PATH_MAX];
    ssize_t len = ::readlink("/proc/self/exe", buf, sizeof(buf)-1);
    if (len != -1) buf[len] = '\0';

    const char* s = basename ? strrchr(buf, '/') : NULL ;  
    return s ? strdup(s+1) : buf ; 
}

#endif


const char* SProc::ExecutableName()
{
    bool basename = true ; 
    return ExecutablePath(basename); 
}










