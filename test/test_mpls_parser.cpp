#include "test_utils.h"
#include "mpls_parser.h"
#include <cassert>
#include <iostream>

void test_mpls_parser() {
    std::cout << "Testing MPLS Parser..." << std::endl;
    
    // Create temporary test directory
    std::string tempDir = createTempTestDir();
    assert(!tempDir.empty());
    
    // Test 1: Invalid MPLS file
    std::string invalidMplsPath = tempDir + "/invalid.mpls";
    std::ofstream invalidFile(invalidMplsPath);
    invalidFile << "This is not a valid MPLS file";
    invalidFile.close();
    
    MPLSInfo mplsInfo;
    bool result = parseMPLS(invalidMplsPath, tempDir, mplsInfo);
    assert(result == false);
    std::cout << "✓ Invalid MPLS file test passed" << std::endl;
    
    // Cleanup
    cleanupTempTestDir(tempDir);
    
    std::cout << "All MPLS Parser tests passed!" << std::endl;
}

int main() {
    test_mpls_parser();
    return 0;
}
