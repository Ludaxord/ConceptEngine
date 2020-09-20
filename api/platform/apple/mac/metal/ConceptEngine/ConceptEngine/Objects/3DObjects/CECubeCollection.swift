//
//  CECubeCollection.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 20/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

class CECubeCollection: CEGameObjectInstances {
    var cubeWidth: Int = 0
    var cubeHeight: Int = 0
    var cubeBack: Int = 0
    
    var time: Float = 0.0
    
    init(cubeWidth: Int, cubeHeight: Int, cubeBack: Int) {
        super.init(meshType: .Cube, instanceCount: cubeWidth * cubeHeight * cubeBack)
        self.cubeWidth = cubeWidth
        self.cubeHeight = cubeHeight
        self.cubeBack = cubeBack
        print("CUBE COUNT: \(cubeWidth * cubeHeight * cubeBack)")
        setColor(CEColorUtils.randomColor)
    }
    
    private func build3DCubeGridCollection(deltaTime: Float, elementsSpace: Float = 1.0, scale: Float = 0.3) {
        let halfWidth: Float = Float(cubeWidth / 2)
        let halfHeight: Float = Float(cubeHeight / 2)
        let halfBack: Float = Float(cubeBack / 2)
        
        var index: Int = 0
        let gap: Float = cos(time / 2) * 10
        for y in stride(from: -halfHeight, to: halfHeight, by: elementsSpace) {
            let yPosition = Float(y * gap)
            for x in stride(from: -halfWidth, to: halfWidth, by: elementsSpace) {
                let xPosition = Float(x * gap)
                for z in stride(from: -halfBack, to: halfBack, by: elementsSpace) {
                    let zPosition = Float(z * gap)
                    _nodes[index].position.y = yPosition
                    _nodes[index].position.x = xPosition
                    _nodes[index].position.z = zPosition
                    
                    _nodes[index].rotation.z -= deltaTime * 2
                    _nodes[index].rotation.y -= deltaTime * 2
                    _nodes[index].scale = float3(scale, scale, scale)
                    index += 1
                }
            }
        }
    }
    
    override func update(deltaTime: Float) {
        time += deltaTime
        build3DCubeGridCollection(deltaTime: deltaTime)
        super.update(deltaTime: deltaTime)
    }
}
