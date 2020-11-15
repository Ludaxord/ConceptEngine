//
//  CEGameObjCarHatch.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 16/10/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

class CEGameObjCarHatch: CEGameObj {
    init(camera: CECamera) {
        super.init(meshType: .CarHatch, camera: camera)
        setName("CarHatch")
    }
}
