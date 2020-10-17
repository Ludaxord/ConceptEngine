//
//  CELight.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 17/10/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

class CELight : CEGameObject {
    
    var lightData = CELightData()
    
    init(name: String, camera: CECamera) {
        super.init(meshType: .None, camera: camera)
        self.setName(name)
    }
    
    init(meshType: MeshTypes, name: String, camera: CECamera) {
        super.init(meshType: meshType, camera: camera)
        super.setName(name)
    }
    
    override func update() {
        self.lightData.position = self.getPosition()
        super.update()
    }
}
