#include <stdio.h>
#include <stdlib.h>
#include "common.h"

using namespace Nan;

#ifdef __DARWIN
  static const char* modelKey = "hw.model";
#endif

#define V8_STRING(value)                                 \
  New<v8::String>(value).ToLocalChecked()                \

#define SET_EXPORT(name, func)                           \
  Set(                                                   \
    target,                                              \
    New<v8::String>(#name).ToLocalChecked(),             \
    New<v8::FunctionTemplate>(func)->GetFunction()       \
  );                                                     \

char* get_device_verison() {
  size_t len = 32;
  char* version = (char*)(malloc(len + 1));
  #ifdef __DARWIN
    FILE *sw_vers = popen("sw_vers -productVersion", "r");
    if (sw_vers) {
      version = fgets(version, (int)len + 1, sw_vers);
      pclose(sw_vers);
      int index = sizeof(version) - 1;
      if (*(version + index) == '\n') {
          *(version + index) = '\0';
      }
      return version;
    }
    strcpy(version, "mac");
  #endif

  #ifdef __WIN32
    #pragma warning(suppress : 4996)
    OSVERSIONINFOW info = { 0 };
    info.dwOSVersionInfoSize = sizeof(info);
    GetVersionExW(&info);
    uint32_t major = info.dwMajorVersion;
    uint32_t minor = info.dwMinorVersion;
    if (info.dwMajorVersion == 6) {
      if (minor == 1) {
        strcpy(version, "windows 7");
      } else if (minor == 2) {
        strcpy(version, "windows 8");
      } else if (minor == 3) {
        strcpy(version, "windows Server 2012 R2");
      } else if (minor == 0) {
        strcpy(version, "windows vista");
      }
    } else if(major == 10) {
      strcpy(version, "windows 10");
    } else {
      strcpy(version, "windows");
    }
  #endif

  return version;
}

NAN_METHOD(Get_Device_Version) {
  char* version = get_device_verison();
  info.GetReturnValue().Set(New<v8::String>(version).ToLocalChecked());
}

char* get_device_model() {
  #ifdef __DARWIN
    size_t len = 0;
    sysctlbyname(modelKey, NULL, &len, NULL, 0);
    char* model = (char*)(malloc(len + 1));
    memset(model, 0, len + 1);
    sysctlbyname(modelKey, model, &len, NULL, 0);
    return model;
  #endif

  #ifdef __WIN32
    int err;
  	HKEY processor_key;
  	WCHAR cpu_brand[256];
  	DWORD cpu_brand_size = sizeof(cpu_brand);
  	err = RegOpenKeyExW(
      HKEY_LOCAL_MACHINE,
  		L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
  		0,
  		KEY_QUERY_VALUE,
  		&processor_key
    );
    if (err != 0) return "";
    err = RegQueryValueExW(
      processor_key,
      L"ProcessorNameString",
      NULL,
      NULL,
      (BYTE*)&cpu_brand,
      &cpu_brand_size
    );
    if (err != 0) return "";
    _bstr_t b(cpu_brand);
    return b;
  #endif
}

uint64_t get_device_memory() {
  uint64_t mem = 0;
  size_t size = sizeof(mem);
  #ifdef __DARWIN
    int which[] = {CTL_HW, HW_MEMSIZE};
    sysctl(which, 2, &mem, &size, NULL, 0);
  #endif

  #ifdef __WIN32
    MEMORYSTATUSEX memory_status;
    memory_status.dwLength = sizeof(memory_status);
    GlobalMemoryStatusEx(&memory_status);
    mem = (uint64_t)memory_status.ullTotalPhys;
  #endif

  return mem >> 30;
}

NAN_METHOD(Get_Device_Type) {
  char* model = get_device_model();
  uint64_t mem = get_device_memory();
  v8::Local<v8::Object> type_obj = Nan::New<v8::Object>();
  Set(type_obj, V8_STRING("model"), V8_STRING(model));
  Set(type_obj, V8_STRING("memory"), New<v8::Number>(mem));

  info.GetReturnValue().Set(type_obj);
}

NAN_MODULE_INIT(Init) {
  SET_EXPORT(getDeviceVersion, Get_Device_Version);
  SET_EXPORT(getDeviceType, Get_Device_Type);
}

NODE_MODULE(device_info, Init);
