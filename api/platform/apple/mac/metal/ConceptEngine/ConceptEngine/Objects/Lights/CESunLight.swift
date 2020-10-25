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
        self.setMaterialColor(float4(0.5, 0.5, 0.0, 1.0))
        self.setScale(0.3)
    }
}
