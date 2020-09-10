//
//  ShaderLibrary.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 10/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

public enum VertexShaderTypes {
    case Basic
}

public enum FragmentShaderTypes {
    case Basic
}

protocol ConceptShader {
    var shaderName: String { get }
    var functionName: String { get }
    var function: MTLFunction { get }
}

public struct BasicVertexShader: ConceptShader {
    public var shaderName: String = "Basic Vertex Shader"
    public var functionName: String = "basic_vertex_shader"
    public var function: MTLFunction {
        return ConceptShaderLibrary.createShaderFunction(functionName: functionName, shaderName: shaderName)
    }
}

public struct BasicFragmentShader: ConceptShader {
    public var shaderName: String = "Basic Fragment Shader"
    public var functionName: String = "basic_fragment_shader"
    public var function: MTLFunction {
        return ConceptShaderLibrary.createShaderFunction(functionName: functionName, shaderName: shaderName)
    }
}

public class ConceptShaderLibrary {
    public static var DefeultLibrary: MTLLibrary!
    
    private static var vertexShaders: [VertexShaderTypes: ConceptShader] = [:]
    private static var fragmentShaders: [FragmentShaderTypes: ConceptShader] = [:]
    
    public static func createShaderFunction(functionName: String, shaderName: String) -> MTLFunction {
        let function = ConceptShaderLibrary.DefeultLibrary.makeFunction(name: functionName)
        function?.label = shaderName
        return function!
    }
    
    public static func Vertex(_ vertexShaderType: VertexShaderTypes) -> MTLFunction {
        return vertexShaders[vertexShaderType]!.function
    }
    
    
    public static func Fragment(_ fragmentShaderType: FragmentShaderTypes) -> MTLFunction {
        return fragmentShaders[fragmentShaderType]!.function
    }
    
    required init(device: MTLDevice) {
        ConceptShaderLibrary.DefeultLibrary = device.makeDefaultLibrary()!
        createDefaultShaders()
    }
    
    public func createDefaultShaders() {
        ConceptShaderLibrary.vertexShaders.updateValue(BasicVertexShader(), forKey: .Basic)
        ConceptShaderLibrary.fragmentShaders.updateValue(BasicFragmentShader(), forKey: .Basic)
    }

}
