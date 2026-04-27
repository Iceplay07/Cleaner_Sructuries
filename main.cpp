#include "../include/Scan.hpp"
#include "../include/Output.hpp"

#include <exception>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

void Print_Usage(const char *program_name) {
    std::cerr << "Usage:\n";
    std::cerr << "  " << program_name << " <directory>\n";
    std::cerr << "  " << program_name << " <directory> -o <file>\n";
    std::cerr << "  " << program_name << " <directory> --output <file>\n";
    std::cerr << "  " << program_name << " <directory> -t <threshold>\n";
    std::cerr << "  " << program_name << " <directory> --threshold <threshold>\n";
    std::cerr << "  " << program_name << " <directory> -t <threshold> -o <file>\n";
    std::cerr << "  " << program_name << " --help\n";
}

int main(int argc, char **argv) {
    if (argc < 2) {
        Print_Usage(argv[0]);
        return 1;
    }

    std::string first_arg = argv[1];

    if (first_arg == "--help" || first_arg == "-h") {
        Print_Usage(argv[0]);
        return 0;
    }

    std::filesystem::path root = argv[1];

    bool to_file = false;
    std::filesystem::path output_path;

    double threshold = 0.80;

    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--help" || arg == "-h") {
            Print_Usage(argv[0]);
            return 0;
        }

        if (arg == "-o" || arg == "--output") {
            if (i + 1 >= argc) {
                std::cerr << "Error: expected file path after " << arg << '\n';
                return 1;
            }

            output_path = argv[i + 1];
            to_file = true;
            ++i;
        } else if (arg == "-t" || arg == "--threshold") {
            if (i + 1 >= argc) {
                std::cerr << "Error: expected number after " << arg << '\n';
                return 1;
            }

            try {
                threshold = std::stod(argv[i + 1]);
            } catch (const std::exception &) {
                std::cerr << "Error: threshold must be a number\n";
                return 1;
            }

            if (threshold < 0.0 || threshold > 1.0) {
                std::cerr << "Error: threshold must be from 0.0 to 1.0\n";
                return 1;
            }

            ++i;
        } else {
            std::cerr << "Error: unknown argument: " << arg << '\n';
            Print_Usage(argv[0]);
            return 1;
        }
    }

    try {
        Scaner scaner(threshold);

        std::vector<Scaner::FileInfo> files = scaner.Scan(root);

        Output output(threshold);

        if (to_file) {
            output.Print_File(files, output_path);
            std::cout << "Report saved to: " << output_path << '\n';
        } else {
            output.Print_Stdout(files);
        }
    } catch (const std::exception &error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}