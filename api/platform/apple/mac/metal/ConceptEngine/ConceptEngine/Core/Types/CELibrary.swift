//
//  CELibrary.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 25/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

public class CELibrary<T, K> {
    init() {
        useLibrary()
    }
    
    func useLibrary() {
        
    }
    
    subscript (_ type: T) -> K? {
        return nil
    }
}
