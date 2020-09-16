//
//  CEBasicAI.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 15/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import Foundation

public class CEBasicAI: CEAI {
    public func followMouseFormula2D(character: CEGameCharacter) -> Float {
        return -atan2f(CEMouse.GetMouseViewportPosition().x - character.position.x, CEMouse.GetMouseViewportPosition().y - character.position.y)
    }
}
