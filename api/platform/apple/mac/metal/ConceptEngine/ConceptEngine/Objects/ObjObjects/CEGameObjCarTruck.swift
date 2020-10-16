//
//  CEGameObjCarTruck.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 16/10/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

class CEGameObjCarTruck: CEGameObj {
    init(camera: CECamera) {
        super.init(meshType: .CarTruck, camera: camera)
        setName("CarTruck")
    }
}
