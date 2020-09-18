//
//  CEGameNPC.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 11/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

class CEGameNPC: CEGameCharacter {
    
    init(camera: CECamera) {
        super.init(meshType: .Triangle, camera: camera)
    }
    
    
    override func update(deltaTime: Float) {
        defaultCharacterAI()
        super.update(deltaTime: deltaTime)
    }
}
