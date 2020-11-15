//
//  Keyboard.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 15/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import Foundation

public class CEKeyboard {
    
    private static var KEY_COUNT: Int = 256
    private static var keys = [Bool].init(repeating: false, count: KEY_COUNT)
    
    public static func SetKeyPressed(_ keyCode: UInt16, isOn: Bool) {
        keys[Int(keyCode)] = isOn
    }
    
    public static func IsKeyPressed(_ keyCode: CEKeyCodes) -> Bool {
        return keys[Int(keyCode.rawValue)]
    }
}
