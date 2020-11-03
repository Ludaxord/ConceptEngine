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
    case Forest
}

public class CESceneManager: CEManager {
    
    private var _currentScene: CEScene!
    
    required init(_ sceneType: SceneTypes) {
        SetScene(sceneType)
    }
    
    public func SetScene(_ sceneType: SceneTypes) {
        switch sceneType {
        case .Sandbox:
            _currentScene = CESandbox(name: "Sandbox")
        case .Forest:
            _currentScene = CEForest(name: "Forest")
        }
    }
    
    public func GenerateScene(renderCommandEncoder: MTLRenderCommandEncoder, deltaTime: Float) {
        CEGameTime.UpdateTime(deltaTime)
        _currentScene.updateCameras()
        _currentScene.update()
        _currentScene.render(renderCommandEncoder: renderCommandEncoder)
    }
}
