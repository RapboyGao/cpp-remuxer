#include "cli_parser.h"
#include <getopt.h>
#include <iostream>
#include <cstring>

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
    
    // Define long options
    static struct option long_options[] = {
        {"input", required_argument, 0, 'i'},
        {"playlist", required_argument, 0, 'p'},
        {"output", required_argument, 0, 'o'},
        {"audio", required_argument, 0, 'a'},
        {"subtitle", required_argument, 0, 's'},
        {"chapters", no_argument, 0, 'c'},
        {"verbose", no_argument, 0, 'v'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };
    
    int option_index = 0;
    int c;
    
    // Parse arguments
    while ((c = getopt_long(argc, argv, "i:p:o:a:s:cv", long_options, &option_index)) != -1) {
        switch (c) {
            case 'i':
                args.input = optarg;
                break;
            case 'p':
                args.playlist = optarg;
                break;
            case 'o':
                args.output = optarg;
                break;
            case 'a':
                args.audio = optarg;
                break;
            case 's':
                args.subtitle = optarg;
                break;
            case 'c':
                args.chapters = true;
                break;
            case 'v':
                args.verbose = true;
                break;
            case 'h':
                printHelp();
                return false;
            case '?':
                // getopt_long already printed an error message
                printHelp();
                return false;
            default:
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
