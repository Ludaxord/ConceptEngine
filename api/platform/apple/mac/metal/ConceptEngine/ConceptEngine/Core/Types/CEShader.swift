//
//  CEShader.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 25/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

class CEShader {
    var function: MTLFunction!
    init(name: String, functionName: String) {
        self.function = ConceptEngine.DefaultLibrary.makeFunction(name: functionName)
        self.function.label = name
    }
}
