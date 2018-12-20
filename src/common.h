#include <nan.h>

#ifdef __WIN32
  #include <windows.h>
  #include <comdef.h> 
#endif

#ifdef __DARWIN
  #include <sys/sysctl.h>
#endif
