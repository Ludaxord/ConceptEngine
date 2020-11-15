//
//  CEVertexShaderLibrary.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 25/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

@available(*, deprecated)
public enum VertexShaderTypes {
    case Basic
    case Instanced
}

@available(*, deprecated)
public final class CEVertexShaderLibrary: CELibrary<VertexShaderTypes, MTLFunction>, CEStandardLibrary {
       
   private var vertexShaders: [VertexShaderTypes: CEShader] = [:]
   
    override func useLibrary() {
        createDefaultShaders()
    }
    
    override subscript(type: VertexShaderTypes) -> MTLFunction? {
        return Vertex(type)
    }
    
   public static func createShaderFunction(functionName: String, shaderName: String) -> MTLFunction {
        let function = ConceptEngine.DefaultLibrary.makeFunction(name: functionName)
       function?.label = shaderName
       return function!
   }
   
   public func Vertex(_ vertexShaderType: VertexShaderTypes) -> MTLFunction {
       return vertexShaders[vertexShaderType]!.function
   }
   
   private func createDefaultShaders() {
       vertexShaders.updateValue(CEShader(name: "Basic Vertex Shader", functionName: "basic_vertex_shader"), forKey: .Basic)
       vertexShaders.updateValue(CEShader(name: "Instanced Vertex Shader", functionName: "instanced_vertex_shader"), forKey: .Instanced)
   }
}
