//
//  CEGameObjQuad.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 01/11/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

class CEGameObjQuad: CEGameObj {
    init(camera: CECamera) {
        super.init(meshType: .NormalQuad, camera: camera)
        setName("Quad")
    }
}

