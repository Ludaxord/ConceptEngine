#include <iostream>
#include <cstdint>

extern "C" void macApiMain();


void switch_os() {
    std::cout << "Host OS:" << std::endl;
#if __APPLE__

#include "TargetConditionals.h"

#if TARGET_OS_IPHONE && TARGET_IPHONE_SIMULATOR
    printf("iPhone stimulator\n");
#elif TARGET_OS_IPHONE
    printf("iPhone\n");
#elif TARGET_OS_MAC
    printf("MacOS\n");
    macApiMain();
#else
    printf("Other Apple OS\n");
#endif
#elif _WIN32
#ifdef _WIN64
    printf("Windows 64 bit\n");
#else
    printf("Windows 32 bit\n");
#endif
#elif __LINUX__
// linux specific code
#elif BSD
// BSD specific code
#else
// general code or warning
#endif
    // general code
}

int main() {
//    TODO: call AppleBridge from C++ main file. Create initial install script in install directory
    std::cout << "Welcome to ConceptEngine..." << std::endl;
    switch_os();
    return 0;
}