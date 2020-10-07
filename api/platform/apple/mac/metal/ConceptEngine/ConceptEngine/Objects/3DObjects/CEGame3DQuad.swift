//
//  CEGame3DQuad.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 07/10/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

public class CEGame3DQuad: CEGame3DObject {
    init(camera: CECamera) {
        super.init(meshType: .Quad, camera: camera)
        self.setName("Quad")
//        self.setScale(0.3)
    }
}
