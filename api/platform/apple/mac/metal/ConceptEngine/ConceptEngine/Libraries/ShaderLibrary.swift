//
//  ShaderLibrary.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 10/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

enum VertexShaderTypes {
    case Basic
}

enum FragmentShaderTypes {
    case Basic
}

protocol Shader {
    var shaderLibrary: ShaderLibrary { get }
    var shaderName: String { get }
    var functionName: String { get }
    var function: MTLFunction { get }
}

public struct BasicVertexShader: Shader {
    public var shaderLibrary: ShaderLibrary = ShaderLibrary()
    public var shaderName: String = "Basic Vertex Shader"
    public var functionName: String = "basic_vertex_shader"
    public var function: MTLFunction {
        return shaderLibrary.createShaderFunction(functionName: functionName, shaderName: shaderName)
    }
}

public struct BasicFragmentShader: Shader {
    public var shaderLibrary: ShaderLibrary = ShaderLibrary()
    public var shaderName: String = "Basic Fragment Shader"
    public var functionName: String = "basic_fragment_shader"
    public var function: MTLFunction {
        return shaderLibrary.createShaderFunction(functionName: functionName, shaderName: shaderName)
    }
}

public class ShaderLibrary {
    public var DefeultLibrary: MTLLibrary
    required init() {
        DefeultLibrary = Engine.Device.makeDefaultLibrary()!
    }
    
    public func createShaderFunction(functionName: String, shaderName: String) -> MTLFunction {
        let function = self.DefeultLibrary.makeFunction(name: functionName)
        function?.label = shaderName
        return function!
    }
}
