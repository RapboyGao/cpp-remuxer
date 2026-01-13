#include "cli_parser.h"
#include <iostream>
#include <string>
#include <map>

static void printHelp() {
    std::cout << "Usage: bdremux [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -i, --input <path>    BDMV root directory" << std::endl;
    std::cout << "  -p, --playlist <name> mpls filename or 'auto'" << std::endl;
    std::cout << "  -o, --output <path>   Output MKV file" << std::endl;
    std::cout << "  -a, --audio <tracks>  Audio track indices or 'all'" << std::endl;
    std::cout << "  -s, --subtitle <tracks> Subtitle track indices or 'all'" << std::endl;
    std::cout << "  -c, --chapters        Write chapters to output" << std::endl;
    std::cout << "  -v, --verbose         Enable verbose output" << std::endl;
    std::cout << "  -h, --help            Show this help message" << std::endl;
}

bool parseCLIArgs(int argc, char* argv[], CLIArgs& args) {
    // Set default values
    args.playlist = "auto";
    args.audio = "all";
    args.subtitle = "all";
    args.chapters = true;
    args.verbose = false;
    
    // Simple argument parsing for Windows
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        // Check for help option
        if (arg == "-h" || arg == "--help") {
            printHelp();
            return false;
        }
        
        // Check for verbose option
        if (arg == "-v" || arg == "--verbose") {
            args.verbose = true;
            continue;
        }
        
        // Check for chapters option
        if (arg == "-c" || arg == "--chapters") {
            args.chapters = true;
            continue;
        }
        
        // Check for options with arguments
        if (i + 1 < argc) {
            if (arg == "-i" || arg == "--input") {
                args.input = argv[++i];
            } else if (arg == "-p" || arg == "--playlist") {
                args.playlist = argv[++i];
            } else if (arg == "-o" || arg == "--output") {
                args.output = argv[++i];
            } else if (arg == "-a" || arg == "--audio") {
                args.audio = argv[++i];
            } else if (arg == "-s" || arg == "--subtitle") {
                args.subtitle = argv[++i];
            } else {
                std::cerr << "Unknown option: " << arg << std::endl;
                printHelp();
                return false;
            }
        } else {
            std::cerr << "Missing argument for option: " << arg << std::endl;
            printHelp();
            return false;
        }
    }
    
    // Validate required arguments
    if (args.input.empty() || args.output.empty()) {
        std::cerr << "Error: --input and --output are required arguments" << std::endl;
        printHelp();
        return false;
    }
    
    return true;
}
