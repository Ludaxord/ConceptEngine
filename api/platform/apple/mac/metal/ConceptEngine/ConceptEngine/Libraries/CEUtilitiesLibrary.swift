//
//  Utilities.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 10/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

public enum ClearColors {
    static let White: MTLClearColor = MTLClearColor(red: 1.0, green: 1.0, blue: 1.0, alpha: 1.0)
    static let Green: MTLClearColor = MTLClearColor(red: 0.22, green: 0.55, blue: 0.34, alpha: 1.0)
    static let Grey: MTLClearColor = MTLClearColor(red: 0.5, green: 0.5, blue: 0.5, alpha: 1.0)
    static let Black: MTLClearColor = MTLClearColor(red: 0.0, green: 0.0, blue: 0.0, alpha: 1.0)
    static let Orange: MTLClearColor = MTLClearColor(red: 0.89, green: 0.29, blue: 0.10, alpha: 1.0)
}

public enum PixelFormats {
    static let Default2DPixelFormat: MTLPixelFormat = MTLPixelFormat.bgra8Unorm
    static let Default3DDepthPixelFormat: MTLPixelFormat = MTLPixelFormat.depth32Float
}

public final class CEUtilitiesLibrary: CEStandardLibrary {
    
    public static var ClearColor: MTLClearColor!
    
    public static var PixelFormat: MTLPixelFormat!
    
    public static var DepthPixelFormat: MTLPixelFormat!
    
    public static var DefaultSceneType: SceneTypes!
    
    required init() {
        CEUtilitiesLibrary.ClearColor = ClearColors.Grey
        CEUtilitiesLibrary.PixelFormat = PixelFormats.Default2DPixelFormat
        CEUtilitiesLibrary.DepthPixelFormat = PixelFormats.Default3DDepthPixelFormat
        CEUtilitiesLibrary.DefaultSceneType = .Sandbox
    }
    
}
