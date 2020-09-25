//
//  CEGameTime.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 25/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

class CEGameTime {
    private static var _totalGameTime: Float = 0.0
    private static var _deltaTime: Float = 0.0
    public static func UpdateTime(_ deltaTime: Float) {
        _deltaTime = deltaTime
        _totalGameTime += deltaTime
    }
}

extension CEGameTime {
    public static var TotalGameTime: Float {
        return _totalGameTime
    }
    
    public static var DeltaTime: Float {
        return _deltaTime
    }
}
