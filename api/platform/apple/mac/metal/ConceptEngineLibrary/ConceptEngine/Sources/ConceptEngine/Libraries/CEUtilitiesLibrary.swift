//
//  Utilities.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 10/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

public enum ClearColors {
    static let White = MTLClearColor(red: 1.0, green: 1.0, blue: 1.0, alpha: 1.0)
    static let Green = MTLClearColor(red: 0.22, green: 0.55, blue: 0.34, alpha: 1.0)
    static let Grey = MTLClearColor(red: 0.5, green: 0.5, blue: 0.5, alpha: 1.0)
    static let DarkGrey = MTLClearColor(red: 0.01, green: 0.01, blue: 0.01, alpha: 1.0)
    static let Black = MTLClearColor(red: 0, green: 0, blue: 0, alpha: 1)
    static let LimeGreen = MTLClearColor(red: 0.3, green: 0.7, blue: 0.3, alpha: 1)
    static let SkyBlue = MTLClearColor(red: 0.3, green: 0.4, blue: 0.8, alpha: 1)
}

public enum PixelFormats {
    static let Default2DPixelFormat: MTLPixelFormat = MTLPixelFormat.bgra8Unorm
    static let DefaultTexturePixelFormat: MTLPixelFormat = MTLPixelFormat.bgra8Unorm_srgb
    static let Default3DDepthPixelFormat: MTLPixelFormat = MTLPixelFormat.depth32Float
}

public final class CEUtilitiesLibrary: CEStandardLibrary {
    
    public static var ClearColor: MTLClearColor!
    
    public static var PixelFormat: MTLPixelFormat!
    
    public static var DepthPixelFormat: MTLPixelFormat!
    
    public static var DefaultSceneType: SceneTypes!
    
    required init() {
        CEUtilitiesLibrary.ClearColor = ClearColors.SkyBlue
//        CEUtilitiesLibrary.PixelFormat = PixelFormats.Default2DPixelFormat
        CEUtilitiesLibrary.PixelFormat = PixelFormats.DefaultTexturePixelFormat
        CEUtilitiesLibrary.DepthPixelFormat = PixelFormats.Default3DDepthPixelFormat
//        CEUtilitiesLibrary.DefaultSceneType = .Sandbox
        CEUtilitiesLibrary.DefaultSceneType = .Forest
    }
    
}
