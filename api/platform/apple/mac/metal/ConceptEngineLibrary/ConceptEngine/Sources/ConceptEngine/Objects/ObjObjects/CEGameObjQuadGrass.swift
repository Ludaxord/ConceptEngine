//
//  CEGameObjQuadGrass.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 01/11/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

class CEGameObjQuadGrass: CEGameObj {
    init(camera: CECamera) {
        super.init(meshType: .QuadGrass, camera: camera)
        setName("QuadGrass")
    }
}
