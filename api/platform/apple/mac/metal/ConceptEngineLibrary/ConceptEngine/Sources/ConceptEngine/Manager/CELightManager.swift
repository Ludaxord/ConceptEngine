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
    
    func addLights(_ lights: [CELight]) {
        self._lightObjects.append(contentsOf: lights)
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
        var lightCount = lightDatas.count
        renderCommandEncoder.setFragmentBytes(&lightCount,
                                              length: Int32.size,
                                              index: 2)
        renderCommandEncoder.setFragmentBytes(&lightDatas,
                                              length: CELightData.stride(lightCount),
                                              index: 3)
    }
}
