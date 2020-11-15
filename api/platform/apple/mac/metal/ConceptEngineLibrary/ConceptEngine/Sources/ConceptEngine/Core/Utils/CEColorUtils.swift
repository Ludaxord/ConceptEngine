//
//  ColorUtils.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 18/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import simd
import Foundation


class CEColorUtils {
    public static var randomColor: float4 {
        return float4(Float.randomFromZeroToOne, Float.randomFromZeroToOne, Float.randomFromZeroToOne, 1.0)
    }
    
    public static func createColor(r: Float, g: Float, b: Float, alpha: Float = 1.0) -> float4 {
        return float4(r, g, b, alpha)
    }
    
    // source: https://stackoverflow.com/a/27203691/7927803
    public static func hexStringToColor(hex: String) -> float4 {
        var cString: String = hex.trimmingCharacters(in: .whitespacesAndNewlines).uppercased()

        if (cString.hasPrefix("#")) {
            cString.remove(at: cString.startIndex)
        }

        if ((cString.count) != 6) {
            return createColor(r: 0.0, g: 0.0, b: 0.0)
        }

        var rgbValue:UInt64 = 0
        Scanner(string: cString).scanHexInt64(&rgbValue)

        return float4(
            Float((rgbValue & 0xFF0000) >> 16) / 255.0,
            Float((rgbValue & 0x00FF00) >> 8) / 255.0,
            Float(rgbValue & 0x0000FF) / 255.0,
            Float(1.0)
        )
    }
}

