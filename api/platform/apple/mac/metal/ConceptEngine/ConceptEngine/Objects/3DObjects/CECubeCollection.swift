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
    var cubeScale: Float = 0.3
    
    private var additionalUpdate = 0
    
    init(cubeWidth: Int, cubeHeight: Int, cubeBack: Int, scale: Float = 0.3) {
        super.init(meshType: .Cube, instanceCount: cubeWidth * cubeHeight * cubeBack)
        self.cubeWidth = cubeWidth
        self.cubeHeight = cubeHeight
        self.cubeBack = cubeBack
        self.cubeScale = scale
        print("CUBE COUNT: \(cubeWidth * cubeHeight * cubeBack)")
        setColor(CEColorUtils.randomColor)
    }
    
    private func build3DCubeGridCollection(elementsSpace: Float = 1.0, scale: Float = 0.3) {
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
                    
                    _nodes[index].rotation.z -= CEGameTime.DeltaTime * 2
                    _nodes[index].rotation.y -= CEGameTime.DeltaTime * 2
                    _nodes[index].scale = float3(scale, scale, scale)
                    index += 1
                }
            }
        }
    }
    
    private func updateCubes() {
        for node in _nodes {
            if additionalUpdate % 100 == 0 {
                setColor(CEColorUtils.randomColor)
            }
        }
    }
    
    override func update() {
        additionalUpdate += 1
        build3DCubeGridCollection(scale: cubeScale)
        updateCubes()
        super.update()
    }
}
