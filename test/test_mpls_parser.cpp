#include "test_utils.h"
#include "mpls_parser.h"
#include <cassert>
#include <iostream>
#include <fstream>

void test_mpls_parser() {
    std::cout << "Testing MPLS Parser..." << std::endl;
    
    // Create temporary test directory
    std::string tempDir = createTempTestDir();
    assert(!tempDir.empty());
    
    // Test 1: Invalid MPLS file
    std::string invalidMplsPath = tempDir + "/invalid.mpls";
    std::ofstream invalidFile(invalidMplsPath);
    (void)(invalidFile << "This is not a valid MPLS file"); // 避免未使用表达式结果警告
    invalidFile.close();
    
    MPLSInfo mplsInfo;
    bool result = parseMPLS(invalidMplsPath, tempDir, mplsInfo);
    if (!result) {
        std::cout << "[OK] Invalid MPLS file test passed" << std::endl;
    } else {
        std::cerr << "[FAILED] Invalid MPLS file test" << std::endl;
        assert(false);
    } // 使用ASCII字符替代非ASCII字符
    
    // Cleanup
    cleanupTempTestDir(tempDir);
    
    std::cout << "All MPLS Parser tests passed!" << std::endl;
}

int main() {
    test_mpls_parser();
    return 0;
}
