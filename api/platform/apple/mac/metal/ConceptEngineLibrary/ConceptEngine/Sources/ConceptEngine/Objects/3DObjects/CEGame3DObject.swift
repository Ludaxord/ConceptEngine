//
//  CEGame3DObject.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 18/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit


public class CEGame3DObject: CEGameObject {
        
    override init(meshType: MeshTypes, meshFillMode: MTLTriangleFillMode = .fill, camera: CECamera) {
        super.init(meshType: meshType, meshFillMode: meshFillMode, camera: camera)
    }
    
}
