//
//  CEGameObjTrees.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 11/11/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import Foundation

class CEGameObjTrees: CENode {
    init(treeACount: Int, treeBCount: Int, treeCCount: Int) {
        super.init(name: "Trees")
        
        let treeAs = CEInstancedGameObject(meshType: .TreePineTallADetailed, instanceCount: treeACount)
        treeAs.updateNodes(updateTreePosition)
        addNodeChild(treeAs)
        
        let treeBs = CEInstancedGameObject(meshType: .TreePineDefaultB, instanceCount: treeBCount)
        treeBs.updateNodes(updateTreePosition)
        addNodeChild(treeBs)
        
        let treeCs = CEInstancedGameObject(meshType: .TreePineRoundC, instanceCount: treeCCount)
        treeCs.updateNodes(updateTreePosition)
        addNodeChild(treeCs)
    }
    
    private func updateTreePosition(tree: CENode, i: Int) {
        let radius: Float = Float.random(in: 8...70)
        let pos = float3(cos(Float(i)) * radius,
                         0,
                         sin(Float(i)) * radius)
        tree.setPosition(pos)
        tree.setScale(Float.random(in: 1...2))
        tree.rotateY(Float.random(in: 0...360))
    }
}
