//
//  CEGameObjGroundGrass.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 05/11/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

class CEGameObjGroundGrass: CEGameObj {
    init(camera: CECamera) {
        super.init(meshType: .GroundGrass, camera: camera)
        setName("Terrain")
    }
}
