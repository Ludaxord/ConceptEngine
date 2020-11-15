//
//  CEGame3DCube.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 18/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

public class CEGame3DCube: CEGame3DObject {
    init(camera: CECamera) {
        super.init(meshType: .Cube, camera: camera)
    }
}
