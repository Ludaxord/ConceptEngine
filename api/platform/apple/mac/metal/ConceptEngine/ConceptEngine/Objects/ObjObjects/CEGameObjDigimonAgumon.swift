//
//  CEGameObjDigimonAgumon.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 27/10/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

class CEGameObjDigimonAgumon: CEGameObj {
    init(camera: CECamera) {
        super.init(meshType: .DigimonAgumon, camera: camera)
        setName("DigimonAgumon")
    }
}
