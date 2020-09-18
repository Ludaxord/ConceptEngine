//
//  CCharacter.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 11/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

public class CEGameCharacter: CEGameObject {
    
    public var CharacterAI: CEAI!
    
    override init(meshType: MeshTypes, meshFillMode: MTLTriangleFillMode = .fill, camera: CECamera) {
        super.init(meshType: meshType, meshFillMode: meshFillMode, camera: camera)
        CharacterAI = CEBasicAI()
    }
    
    public func defaultCharacterAI() {
        self.rotation.z = self.followCharacterMouse2D
    }
    
}


extension CEGameCharacter {
    var followCharacterMouse2D: Float { return CharacterAI.followMouseFormula2D(character: self, camera: self.camera) }
}
