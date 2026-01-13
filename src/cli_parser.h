#ifndef CLI_PARSER_H
#define CLI_PARSER_H

#include <string>
#include <vector>

struct CLIArgs {
    std::string input;      // BDMV root directory
    std::string playlist;   // mpls filename or "auto"
    std::string output;     // Output MKV file path
    std::string audio;      // Audio track indices or "all"
    std::string subtitle;   // Subtitle track indices or "all"
    bool chapters;          // Whether to write chapters
    bool verbose;           // Verbose output
};

/**
 * Parse command line arguments
 * @param argc Number of arguments
 * @param argv Array of argument strings
 * @param args Reference to CLIArgs structure to fill
 * @return True if parsing succeeded, false otherwise
 */
bool parseCLIArgs(int argc, char* argv[], CLIArgs& args);

#endif // CLI_PARSER_H
