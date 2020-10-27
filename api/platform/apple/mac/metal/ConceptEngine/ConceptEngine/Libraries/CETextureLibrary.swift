//
//  CETextureLibrary.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 14/10/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

public enum TextureTypes {
    case None
    case CarTest
    case CarTruck
    case CarSport
    case CarHatch
    case Monkeys
    case DigimonAgumon
}

public enum TextureOrigin {
    case TopLeft
    case BottomLeft
    case TopRight
    case BottomRight
}

public class CETextureLoader {
    private var _textureName: String!
    private var _textureExtension: String!
    private var _origin: MTKTextureLoader.Origin!
    
    init(textureName: String, textureExtension: String = "png", origin: MTKTextureLoader.Origin = .topLeft) {
        self._textureName = textureName
        self._textureExtension = textureExtension
        self._origin = origin
    }
    
    public func loadTextureFromBundle() -> MTLTexture {
        var texture: MTLTexture!
        
        if let url = Bundle.main.url(forResource: self._textureName, withExtension: self._textureExtension) {
            let defaultTextureLoader = MTKTextureLoader(device: ConceptEngine.GPUDevice)
            let options: [MTKTextureLoader.Option : Any] = [MTKTextureLoader.Option.origin: _origin as Any, MTKTextureLoader.Option.generateMipmaps: true]
            do {
                texture = try defaultTextureLoader.newTexture(URL: url, options: options)
                texture.label = _textureName
            } catch let error as NSError {
                print("ERROR::CREATING::TEXTURE::__\(String(describing: _textureName))__::\(error)")
            }
        } else {
            print("ERROR::CREATING::TEXTURE::__\(String(describing: _textureName)) does not exist")
        }
        
        return texture
    }
    
}

public final class CETexture {
    var texture: MTLTexture!
    
    init(_ textureName: String, fileExtension: String = "png", origin: MTKTextureLoader.Origin = .topLeft) {
        let textureLoader = CETextureLoader(textureName: textureName, textureExtension: fileExtension, origin: origin)
        let texture: MTLTexture = textureLoader.loadTextureFromBundle()
        setTexture(texture)
    }
    
    internal func setTexture(_ texture: MTLTexture) {
        self.texture = texture
    }
    
}

final class CETextureLibrary: CELibrary<TextureTypes, MTLTexture>, CEStandardLibrary {
    private var textures: [TextureTypes: CETexture] = [:]
        
    override func useLibrary() {
        createDefaultTextures()
    }
    
    override subscript(_ type: TextureTypes) -> MTLTexture? {
        return textures[type]?.texture
    }
    
    private func createDefaultTextures() {
        textures.updateValue(CETexture("supra_car", fileExtension: "jpeg"), forKey: .CarTest)
        textures.updateValue(CETexture("zuk", fileExtension: "jpeg", origin: .bottomLeft), forKey: .CarTruck)
        textures.updateValue(CETexture("golf", fileExtension: "png", origin: .flippedVertically), forKey: .CarHatch)
        textures.updateValue(CETexture("Agumon", fileExtension: "png", origin: .bottomLeft), forKey: .DigimonAgumon)
    }
    
    public func Texture(_ textureType: TextureTypes) -> MTLTexture {
        return textures[textureType]!.texture
    }
}

