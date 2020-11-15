//
//  CESunLight.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 17/10/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

class CESunLight: CELight {
    init(camera: CECamera) {
        super.init(
            meshType: .Sphere,
            name: "Sun Light",
            camera: camera)
        self.setScale(0.3)
    }
}
