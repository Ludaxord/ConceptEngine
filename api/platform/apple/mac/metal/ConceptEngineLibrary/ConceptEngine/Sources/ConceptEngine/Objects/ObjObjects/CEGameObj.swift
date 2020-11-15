//
//  CEGameObj.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 16/10/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

public class CEGameObj: CEGame3DObject {
    init(meshType: MeshTypes, camera: CECamera, name: String = String(describing: CEGameObj.self)) {
        super.init(meshType: meshType, camera: camera)
        setName(name)
    }
}
