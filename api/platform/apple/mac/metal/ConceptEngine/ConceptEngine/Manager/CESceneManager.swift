//
//  CESceneManager.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 15/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

public enum SceneTypes {
    case Sandbox
}

public class CESceneManager: CEManager {
    
    private var _currentScene: CEScene!
    
    required init(_ sceneType: SceneTypes) {
        SetScene(sceneType)
    }
    
    public func SetScene(_ sceneType: SceneTypes) {
        switch sceneType {
        case .Sandbox:
            _currentScene = CESandbox()
        }
    }
    
    public func GenerateScene(renderCommandEncoder: MTLRenderCommandEncoder, deltaTime: Float) {
        _currentScene.update(deltaTime: deltaTime)
        _currentScene.render(renderCommandEncoder: renderCommandEncoder)
    }
}
