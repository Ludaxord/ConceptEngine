//
//  CEGamePlayer.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 11/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

class CEGame2DQuad: CEGame2DObject {
    
    init(camera: CECamera) {
        super.init(meshType: .Quad, camera: camera)
    }    
}
