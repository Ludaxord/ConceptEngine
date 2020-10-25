//
//  CEMouse.swift
//  ConceptEngine
//
//  Created by Konrad Uciechowski on 15/09/2020.
//  Copyright © 2020 Ldx Studio. All rights reserved.
//

import MetalKit

public enum MOUSE_BUTTON_CODES: Int {
    case LEFT = 0
    case RIGHT = 1
    case CENTER = 2
}

public class CEMouse {
    private static var MOUSE_BUTTON_COUNT = 12
    private static var mouseButtonList = [Bool].init(repeating: false, count: MOUSE_BUTTON_COUNT)

    private static var overallMousePosition = float2(repeating: 0)
    private static var mousePositionDelta = float2(repeating: 0)

    private static var scrollWheelPosition: Float = 0
    private static var lastWheelPosition: Float = 0.0

    private static var scrollWheelChange: Float = 0.0

    public static func SetMouseButtonPressed(button: Int, isOn: Bool){
        mouseButtonList[button] = isOn
    }

    public static func IsMouseButtonPressed(button: MOUSE_BUTTON_CODES)->Bool{
        return mouseButtonList[Int(button.rawValue)] == true
    }

    public static func SetOverallMousePosition(position: float2){
        self.overallMousePosition = position
    }

    ///Sets the delta distance the mouse had moved
    public static func SetMousePositionChange(overallPosition: float2, deltaPosition: float2){
        self.overallMousePosition = overallPosition
        self.mousePositionDelta = deltaPosition
    }

    public static func ScrollMouse(deltaY: Float){
        scrollWheelPosition += deltaY
        scrollWheelChange += deltaY
    }

    //Returns the overall position of the mouse on the current window
    public static func GetMouseWindowPosition()->float2{
        return overallMousePosition
    }

    ///Returns the movement of the wheel since last time getDWheel() was called
    public static func GetDWheel()->Float{
        let position = scrollWheelChange
        scrollWheelChange = 0
        return position
    }
    
    public static func IsDWheelChanged() -> Bool {
        return GetDWheel() != scrollWheelChange
    }
    
    public static func GetDWheelPosition() -> Float {
        return scrollWheelPosition
    }
    
    public static func GetDWheelChange() -> Float {
        return scrollWheelChange
    }

    ///Movement on the y axis since last time getDY() was called.
    public static func GetDY()->Float{
        let result = mousePositionDelta.y
        mousePositionDelta.y = 0
        return result
    }

    ///Movement on the x axis since last time getDX() was called.
    public static func GetDX()->Float{
        let result = mousePositionDelta.x
        mousePositionDelta.x = 0
        return result
    }

    //Returns the mouse position in screen-view coordinates [-1, 1]
    public static func GetMouseViewportPosition()->float2{
        let xScreenMouse = (overallMousePosition.x - CERenderer.ScreenSize.x * 0.5)
        let yScreenMouse = (overallMousePosition.y - CERenderer.ScreenSize.y * 0.5)
        let xScreen = (CERenderer.ScreenSize.x * 0.5)
        let yScreen = (CERenderer.ScreenSize.y * 0.5)
        let x = xScreenMouse / xScreen
        let y = yScreenMouse / yScreen
        return float2(x, y)
    }
}

