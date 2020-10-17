//
//  CELightManager.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 17/10/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit


public class CELightManager: CEManager {
    private var _lightObjects: [CELight] = []
    
    func addLight(_ light: CELight) {
        self._lightObjects.append(light)
    }
    
    func gatherLightData() -> [CELightData] {
        var result: [CELightData] = []
        
        for light in _lightObjects {
            result.append(light.lightData)
        }
        
        return result
    }
    
    func setLightData(_ renderCommandEncoder: MTLRenderCommandEncoder) {
        var lightDatas = gatherLightData()
        renderCommandEncoder.setFragmentBytes(&lightDatas, length: CELightData.size(lightDatas.count), index: 2)
    }
}
