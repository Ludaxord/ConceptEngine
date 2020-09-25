//
//  ShaderLibrary.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 10/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

public enum FragmentShaderTypes {
    case Basic
}

//public protocol CEShader {
//    var shaderName: String { get }
//    var functionName: String { get }
//    var function: MTLFunction! { get }
//}

//public struct BasicVertexShader: CEShader {
//    public var shaderName: String = "Basic Vertex Shader"
//    public var functionName: String = "basic_vertex_shader"
//    public var function: MTLFunction!
//    init(){
//        function = CEFragmentShaderLibrary.createShaderFunction(functionName: functionName, shaderName: shaderName)
//    }
//}

//public struct BasicFragmentShader: CEShader {
//    public var shaderName: String = "Basic Fragment Shader"
//    public var functionName: String = "basic_fragment_shader"
//    public var function: MTLFunction!
//    init(){
//        function = CEFragmentShaderLibrary.createShaderFunction(functionName: functionName, shaderName: shaderName)
//    }
//}

//public struct InstancedVertexShader: CEShader {
//    public var shaderName: String = "Instanced Vertex Shader"
//    public var functionName: String = "instanced_vertex_shader"
//    public var function: MTLFunction!
//    init(){
//        function = CEFragmentShaderLibrary.createShaderFunction(functionName: functionName, shaderName: shaderName)
//    }
//}

public final class CEFragmentShaderLibrary: CELibrary<FragmentShaderTypes, MTLFunction>, CEStandardLibrary {
        
    private var vertexShaders: [VertexShaderTypes: CEShader] = [:]
    private var fragmentShaders: [FragmentShaderTypes: CEShader] = [:]
    
    override func useLibrary() {
        createDefaultShaders()
    }
    
    override subscript(type: FragmentShaderTypes) -> MTLFunction? {
        return Fragment(type)
    }

    
    public static func createShaderFunction(functionName: String, shaderName: String) -> MTLFunction {
        let function = ConceptEngine.DefaultLibrary.makeFunction(name: functionName)
        function?.label = shaderName
        return function!
    }
    
    public func Fragment(_ fragmentShaderType: FragmentShaderTypes) -> MTLFunction {
        return fragmentShaders[fragmentShaderType]!.function
    }
    
    private func createDefaultShaders() {        
        fragmentShaders.updateValue(CEShader(name: "Basic Fragment Shader", functionName: "basic_fragment_shader"), forKey: .Basic)
    }

}
