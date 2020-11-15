import XCTest
@testable import ConceptEngine

final class ConceptEngineTests: XCTestCase {
    func testExample() {
        // This is an example of a functional test case.
        // Use XCTAssert and related functions to verify your tests produce the correct
        // results.
        XCTAssertEqual(ConceptEngine().text, "Hello, World!")
    }

    static var allTests = [
        ("testExample", testExample),
    ]
}
