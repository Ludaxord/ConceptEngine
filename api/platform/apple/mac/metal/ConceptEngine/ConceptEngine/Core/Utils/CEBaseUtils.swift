//
//  CEBaseUtils.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 12/10/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import Foundation
import MetalKit


class CEBaseUtils {
    
    
    public static func findNotEqualArrays(elements: Array<Int>) -> Array<Int> {
        let tempArr = CEMathUtils.createCombinationsArray(allElements: elements)
        var vunelableIndexes: Array<Int> = []
        for t in tempArr {
            var tempElements: Array<Int> = []
            for x in t {
                tempElements.append(elements[x])
            }
            if !tempElements.allEqual() {
                vunelableIndexes += t
            }
        }
        return vunelableIndexes.duplicates
    }
    
    public static func fillNotEqualArrays(elements: Array<Array<Any>>, completion: (Bool, Array<Int>?)->()) {
        let allElements = elements.map { $0.count }
        let elementsEqual = allElements.allEqual()
        if !elementsEqual {
            let notEqualArrays = CEBaseUtils.findNotEqualArrays(elements: allElements)
            completion(elementsEqual, notEqualArrays)
        } else {
            completion(elementsEqual, nil)
        }
    }
    public static func fillFromType<T>(of count: Int, in x:[T]) -> [T] {
        var filled: [T] = x
        let mCount = count - filled.count
        if T.self == float3.self {
            for _ in 0...mCount - 1 {
                filled.append(float3( 0, 1, 0) as! T)
            }
        } else if T.self == float4.self {
            for _ in 0...mCount - 1 {
                filled.append(float4(0, 0, 1, 1) as! T)
            }
        } else if T.self == float2.self {
            for _ in 0...mCount - 1 {
                filled.append(float2(0) as! T)
            }
        }
        return filled
    }
    
    public static func cast<T>(value: Any, to type: T) -> T? {
        return value as? T
    }
    
    public static func unsafeCopyToTuple<ElementType, Tuple>(array: inout Array<ElementType>, to tuple: inout Tuple) {
        withUnsafeMutablePointer(to: &tuple) { pointer in
            let bound = pointer.withMemoryRebound(to: ElementType.self, capacity: array.count) { $0 }
            array.enumerated().forEach { (bound + $0.offset).pointee = $0.element }
        }
    }
}

//Source: https://stackoverflow.com/a/29588187/7927803
extension Array where Element : Equatable {
    func allEqual() -> Bool {
        if let firstElem = first {
            return !dropFirst().contains { $0 != firstElem }
        }
        return true
    }
}

//Source: https://stackoverflow.com/a/36714828/7927803
extension Array where Element: Comparable {
    func containsSameElements(as other: [Element]) -> Bool {
        return self.count == other.count && self.sorted() == other.sorted()
    }
}

extension Array where Element: Hashable {
    //Source: https://stackoverflow.com/a/25739498/7927803
    var uniques: Array {
        var buffer = Array()
        var added = Set<Element>()
        for elem in self {
            if !added.contains(elem) {
                buffer.append(elem)
                added.insert(elem)
            }
        }
        return buffer
    }
    
    //Source: https://stackoverflow.com/a/51057578/7927803
    var duplicates: Array {
         let groups = Dictionary(grouping: self, by: {$0})
         let duplicateGroups = groups.filter {$1.count > 1}
         let duplicates = Array(duplicateGroups.keys)
         return duplicates
     }
}

