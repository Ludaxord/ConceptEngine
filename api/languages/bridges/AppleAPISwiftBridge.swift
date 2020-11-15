import Cocoa
import AppKit
import MetalKit
import Metal

let app = NSApplication.shared

@_cdecl("macApiMain")
public func macApiMain() {
    print("Swift Concept Engine Bridge")
    let delegate = AppDelegate()
    app.delegate = delegate
    app.run()
}


class AppDelegate: NSObject, NSApplicationDelegate, MTKViewDelegate {

    let window = NSWindow(contentRect: NSMakeRect(1280, 720, 1280, 720),
            styleMask: [.titled, .closable, .miniaturizable, .resizable],
            backing: .buffered,
            defer: false,
            screen: nil)

    private func createDefaultWindow() {
        window.makeKeyAndOrderFront(nil)
        let field = NSTextView(frame: window.contentView!.bounds)
        field.backgroundColor = .white
        field.isContinuousSpellCheckingEnabled = true
        window.contentView?.addSubview(field)
        DispatchQueue(label: "background").async {
            while let str = readLine(strippingNewline: false) {
                DispatchQueue.main.async {
                    field.textStorage?.append(NSAttributedString(string: str))
                }
            }
            app.terminate(self)
        }
    }

    private var ceView: MTKView {
        window.contentView = MTKView()
        return window.contentView as! MTKView
    }

    private func createConceptEngineWindow() {
        guard let device = MTLCreateSystemDefaultDevice() else {
            print("Metal is not supported on this device")
            return
        }

        ceView.device = device
        ceView.delegate = self
        ceView.depthStencilPixelFormat = .depth32Float
        ceView.clearDepth = 1.0

        mtkView(ceView, drawableSizeWillChange: ceView.drawableSize)
    }

    func applicationShouldTerminateAfterLastWindowClosed(_ sender: NSApplication) -> Bool {
        return true
    }

    func applicationDidFinishLaunching(_ notification: Notification) {
        createDefaultWindow()
        createConceptEngineWindow()
    }

    //TODO: create library from ConceptEngine MacOS and remove MTKViewDelegate from AppDelegate
    func mtkView(_ view: MTKView, drawableSizeWillChange size: CGSize) {

    }

    func draw(in view: MTKView) {

    }
}