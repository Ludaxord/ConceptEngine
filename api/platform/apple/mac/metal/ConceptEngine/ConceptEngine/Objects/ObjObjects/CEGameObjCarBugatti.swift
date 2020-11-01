//
//  CEGameObjCarBugatti.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 01/11/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

class CEGameObjCarBugatti: CEGameObj {
    init(camera: CECamera) {
        super.init(meshType: .CarBugatti, camera: camera)
        setName("CarBugatti")
    }
}
