//
//  CEGameObjFlowers.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 11/11/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

class CEGameObjFlowers: CENode {
    init(flowerRedCount: Int, flowerPurpleCount: Int, flowerYellowCount: Int) {
        super.init(name: "Flowers")
        
        let flowerReds = CEInstancedGameObject(meshType: .FlowerRedA, instanceCount: flowerRedCount)
        flowerReds.updateNodes(updateFlowerPosition)
        addNodeChild(flowerReds)
        
        let flowerPurples = CEInstancedGameObject(meshType: .FlowerPurpleA, instanceCount: flowerPurpleCount)
        flowerPurples.updateNodes(updateFlowerPosition)
        addNodeChild(flowerPurples)
        
        let flowerYellows = CEInstancedGameObject(meshType: .FlowerYellowA, instanceCount: flowerYellowCount)
        flowerYellows.updateNodes(updateFlowerPosition)
        addNodeChild(flowerYellows)
    }
    
    private func updateFlowerPosition(flower: CENode, i: Int) {
        let radius: Float = Float.random(in: 0.9...70)
        let pos = float3(cos(Float(i)) * radius,
                         0,
                         sin(Float(i)) * radius)
        flower.setPosition(pos)
        flower.rotateY(Float.random(in: 0...360))
        flower.setScale(Float.random(in: 0.6...0.8))
    }
}
