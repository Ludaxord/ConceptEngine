//
//  main.cpp
//  MacConceptEngineOpenGL
//
//  Created by Konrad Uciechowski on 14/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

#include <iostream>
#include "CEMacOpenGL.h"
int main(int argc, const char * argv[]) {
    std::cout << "Mac Concept Engine OpenGL\n";
    CEMacOpenGL* macOpenGL = new CEMacOpenGL();
    macOpenGL->createWindow();
    return 0;
}
