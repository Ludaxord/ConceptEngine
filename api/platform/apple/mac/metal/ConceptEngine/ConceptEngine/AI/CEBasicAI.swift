//
//  CEBasicAI.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 15/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import Foundation

public class CEBasicAI: CEAI {
    public func followMouseFormula2D(character: CEGameCharacter, camera: CECamera?) -> Float {
        var xPosition: Float = CEMouse.GetMouseViewportPosition().x - character.position.x
        var yPosition: Float = CEMouse.GetMouseViewportPosition().y - character.position.y
        if camera != nil {
            xPosition += camera!.position.x
            yPosition += camera!.position.y
        }
        return -atan2f(xPosition, yPosition)
    }
}
