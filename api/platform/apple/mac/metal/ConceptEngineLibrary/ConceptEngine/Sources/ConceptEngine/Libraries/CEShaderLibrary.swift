//
//  CEShaderLibrary.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 25/10/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

public enum ShaderTypes {
    
    //Vertex
    case BasicVertex
    case InstancedVertex
    
    //Fragment
    case BasicFragment
}

public final class CEShaderLibrary: CELibrary<ShaderTypes, MTLFunction>, CEStandardLibrary {
        
    private var shaders: [ShaderTypes: CEShader] = [:]
    
     override func useLibrary() {
         createDefaultShaders()
     }
     
     override subscript(type: ShaderTypes) -> MTLFunction? {
        return Shader(type)
     }
     
    public static func createShaderFunction(functionName: String, shaderName: String) -> MTLFunction {
         let function = ConceptEngine.DefaultLibrary.makeFunction(name: functionName)
        function?.label = shaderName
        return function!
    }
    
    public func Shader(_ vertexShaderType: ShaderTypes) -> MTLFunction {
        return shaders[vertexShaderType]!.function
    }
    
    private func createDefaultShaders() {
        shaders.updateValue(CEShader(name: "Basic Vertex Shader", functionName: "basic_vertex_shader"), forKey: .BasicVertex)
        shaders.updateValue(CEShader(name: "Instanced Vertex Shader", functionName: "instanced_vertex_shader"), forKey: .InstancedVertex)
        shaders.updateValue(CEShader(name: "Basic Fragment Shader", functionName: "basic_fragment_shader"), forKey: .BasicFragment)
    }
}
