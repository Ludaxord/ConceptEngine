//
//  CEGameObjMonkeys.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 27/10/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

class CEGameObjMonkeys: CEGameObj {
    init(camera: CECamera) {
        super.init(meshType: .Monkeys, camera: camera)
        setName("Monkeys")
    }
}
