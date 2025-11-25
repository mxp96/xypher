#include "Common.h"
#include "XypherConfig.h"
#include "ast/ASTDumper.h"
#include "backend/Optimizer.h"
#include "codegen/CodeGenerator.h"
#include "frontend/Diagnostics.h"
#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "sema/SemanticAnalyzer.h"

#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#ifndef MAX_PATH
#define MAX_PATH 260
#endif
#else
#include <sys/wait.h>
#include <unistd.h>
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif
#endif

namespace fs = std::filesystem;

using namespace xypher;

struct CompilerOptions {
    String inputFile;
    String outputFile = "output";
    bool emitLLVM = false;
    bool emitASM = false;
    bool checkSyntaxOnly = false;
    bool dumpAST = false;
    bool debugMode = false;
    int optLevel = 0;
    bool showHelp = false;
    bool showVersion = false;
    bool verifyIR = false;
    bool printOptStats = false;
    bool emitOptimizedIR = false;
    bool useEnhancedPipeline = true; // Use new pipeline by default
};

void printHelp() {
    std::cout << "Xypher Compiler (xypc) v" << XYPHER_VERSION_STRING << "\n\n";
    std::cout << "Usage: xypc [options] <input.xyp>\n\n";
    std::cout << "Options:\n";
    std::cout << "  -o <file>          Output file name\n";
    std::cout << "  --emit-llvm        Emit LLVM IR (optimized if -O used)\n";
    std::cout << "  --emit-opt-ir      Emit optimized LLVM IR\n";
    std::cout << "  --check-syntax     Syntax check only\n";
    std::cout << "  --ast-dump         Dump AST\n";
    std::cout << "  --verify-ir        Verify IR after optimization\n";
    std::cout << "  --print-stats      Print optimization statistics\n";
    std::cout << "  -O<0-3>            Optimization level (with LTO)\n";
    std::cout << "  -Os                Optimize for size (with LTO)\n";
    std::cout << "  -Oz                Aggressive size optimization\n";
    std::cout << "  --size             Maximum size reduction\n";
    std::cout << "  --legacy-opt       Use legacy optimization pipeline\n";
    std::cout << "  -h, --help         Show help\n";
    std::cout << "  -v, --version      Show version\n";
    std::cout << "\n";
    std::cout << "Examples:\n";
    std::cout << "  xypc main.xyp -o main\n";
    std::cout << "  xypc program.xyp -O2 -o fast\n";
    std::cout << "  xypc program.xyp -Os -o small\n";
}

void printVersion() {
    std::cout << "Xypher Compiler (xypc) v" << XYPHER_VERSION_STRING << "\n";
    std::cout << "Simple compiled language with LLVM backend\n";
}

CompilerOptions parseArguments(int argc, char* argv[]) {
    CompilerOptions opts;

    for (int i = 1; i < argc; i++) {
        String arg = argv[i];

        if (arg == "-h" || arg == "--help") {
            opts.showHelp = true;
        } else if (arg == "-v" || arg == "--version") {
            opts.showVersion = true;
        } else if (arg == "-o") {
            if (i + 1 < argc) {
                opts.outputFile = argv[++i];
            }
        } else if (arg == "--emit-llvm") {
            opts.emitLLVM = true;
        } else if (arg == "--emit-opt-ir") {
            opts.emitOptimizedIR = true;
        } else if (arg == "--emit-asm") {
            opts.emitASM = true;
        } else if (arg == "--check-syntax") {
            opts.checkSyntaxOnly = true;
        } else if (arg == "--ast-dump") {
            opts.dumpAST = true;
        } else if (arg == "--verify-ir") {
            opts.verifyIR = true;
        } else if (arg == "--print-stats") {
            opts.printOptStats = true;
        } else if (arg == "--legacy-opt") {
            opts.useEnhancedPipeline = false;
        } else if (arg == "--debug") {
            opts.debugMode = true;
        } else if (arg == "--size") {
            opts.optLevel = 5; // Force maximum size optimization
        } else if (arg.substr(0, 2) == "-O") {
            if (arg == "-Os") {
                opts.optLevel = 4; // Focus on smaller binaries
            } else if (arg == "-Oz") {
                opts.optLevel = 5; // Maximum size reduction
            } else if (arg.length() == 3) {
                opts.optLevel = arg[2] - '0';
            }
        } else if (arg[0] != '-') {
            opts.inputFile = arg;
        }
    }

    return opts;
}

String readFile(const String& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file: " << filename << "\n";
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool createDirectoryIfNeeded(const String& filepath) {
    fs::path p(filepath);
    if (p.has_parent_path()) {
        fs::path parent = p.parent_path();
        if (!parent.empty() && !fs::exists(parent)) {
            try {
                fs::create_directories(parent);
                return true;
            } catch (const std::exception& e) {
                std::cerr << "Error: Could not create directory: " << parent.string() << "\n";
                std::cerr << "Reason: " << e.what() << "\n";
                return false;
            }
        }
    }
    return true;
}

fs::path getExecutablePath() {
#ifdef _WIN32
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    return fs::path(path);
#else
    char path[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", path, PATH_MAX);
    if (count != -1) {
        path[count] = '\0';
        return fs::path(path);
    }
    // Fallback for macOS
    return fs::path(getenv("_") ? getenv("_") : ".");
#endif
}

fs::path findStdLibPath() {
    // Get compiler executable path
    fs::path exePath = getExecutablePath();
    fs::path exeDir = exePath.parent_path();

    // Possible locations for xystd library:
    // 1. Same directory as compiler
    // 2. ../lib/ from compiler
    // 3. ../bin/ from compiler (build directory structure)
    // 4. ../../bin/ from compiler (if in Release/Debug subfolder)

    std::vector<fs::path> searchPaths = {
        exeDir,                       // Same directory
        exeDir / ".." / "bin",        // ../bin
        exeDir / ".." / ".." / "bin", // ../../bin (for VS build)
        exeDir / "lib",               // ./lib
        exeDir / ".." / "lib",        // ../lib
    };

#ifdef _WIN32
    String libName = "xystd.dll";
#elif defined(__APPLE__)
    String libName = "libxystd.dylib";
#else
    String libName = "libxystd.so";
#endif

    for (const auto& searchPath : searchPaths) {
        fs::path libPath = searchPath / libName;
        if (fs::exists(libPath)) {
            return searchPath;
        }
    }

    // Not found, return exe directory as fallback
    return exeDir;
}

String getCompileFlags(int optLevel) {
    String compileFlags;

    if (optLevel >= 2 || optLevel == 4 || optLevel == 5) {
#if defined(_WIN32)
        compileFlags = "-O2 -flto -ffunction-sections -fdata-sections";
#else
        compileFlags = "-O2 -flto -ffunction-sections -fdata-sections";
#endif
    } else if (optLevel == 1) {
        compileFlags = "-O1 -flto";
    }

    return compileFlags;
}

String getLinkFlags(int optLevel) {
    String linkFlags;

    if (optLevel >= 2 || optLevel == 4 || optLevel == 5) {
#if defined(_WIN32)
        linkFlags = "-flto -Wl,/LTCG -Wl,/OPT:REF -Wl,/OPT:ICF";
#else
        linkFlags = "-flto -Wl,--gc-sections -s";
#endif
    } else if (optLevel == 1) {
#if defined(_WIN32)
        linkFlags = "-flto -Wl,/LTCG";
#else
        linkFlags = "-flto";
#endif
    }

    return linkFlags;
}

bool linkExecutable(const String& objFile, const String& exeFile, int optLevel) {
    fs::path stdLibPath = findStdLibPath();

    String compileFlags = getCompileFlags(optLevel);
    String linkFlags = getLinkFlags(optLevel);

#ifdef _WIN32
    String dllPath = (stdLibPath / "xystd.dll").string();

    if (!fs::exists(dllPath)) {
        String command =
            "clang " + compileFlags + " " + objFile + " " + linkFlags + " -o " + exeFile + ".exe";
        return system(command.c_str()) == 0;
    }

    String command = "clang " + compileFlags + " " + objFile + " -L\"" + stdLibPath.string() +
                     "\" -lxystd " + linkFlags + " -o " + exeFile + ".exe";
#else
#ifdef __APPLE__
    String libName = "libxystd.dylib";
#else
    String libName = "libxystd.so";
#endif

    String libPath = (stdLibPath / libName).string();

    if (!fs::exists(libPath)) {
        String command =
            "clang " + compileFlags + " " + objFile + " " + linkFlags + " -o " + exeFile;
        return system(command.c_str()) == 0;
    }

    String command = "clang " + compileFlags + " " + objFile + " -L\"" + stdLibPath.string() +
                     "\" -lxystd -Wl,-rpath,\"" + stdLibPath.string() + "\" " + linkFlags + " -o " +
                     exeFile;
#endif

    return system(command.c_str()) == 0;
}

int main(int argc, char* argv[]) {
    CompilerOptions opts = parseArguments(argc, argv);

    if (opts.showHelp) {
        printHelp();
        return 0;
    }

    if (opts.showVersion) {
        printVersion();
        return 0;
    }

    if (opts.inputFile.empty()) {
        std::cerr << "Error: No input file specified\n";
        std::cerr << "Use 'xypc --help' for usage information\n";
        return 1;
    }

    String source = readFile(opts.inputFile);
    if (source.empty()) {
        std::cerr << "Error: Cannot read file: " << opts.inputFile << "\n";
        return 1;
    }

    DiagnosticEngine diags;
    Lexer lexer(source, opts.inputFile);
    Parser parser(lexer, diags);

    auto program = parser.parseProgram();

    if (diags.hasErrors()) {
        return 1;
    }

    if (opts.checkSyntaxOnly) {
        std::cout << "Syntax OK\n";
        return 0;
    }

    if (opts.dumpAST) {
        ASTDumper dumper;
        dumper.dump(program.get());
        return 0;
    }

    SemanticAnalyzer analyzer(diags);
    if (!analyzer.analyze(program.get())) {
        return 1;
    }

    CodeGenerator codegen(opts.inputFile, diags);

    if (!codegen.generate(program.get())) {
        return 1;
    }

    // Run LLVM IR optimization passes
    if (opts.optLevel > 0) {
        OptimizationLevel level = static_cast<OptimizationLevel>(opts.optLevel);

        String optStr = "O" + std::to_string(opts.optLevel);
        if (opts.optLevel == 4)
            optStr = "Os";
        if (opts.optLevel == 5)
            optStr = "Oz";
        std::cout << "Optimizing (" << optStr << ")...\n";

        if (opts.useEnhancedPipeline) {
            Optimizer::optimizeWithPipeline(codegen.getModule(), level);
        } else {
            std::cout << "[Using legacy optimization pipeline]\n";
            Optimizer::optimize(codegen.getModule(), level);
        }

        // Verify IR if requested
        if (opts.verifyIR) {
            std::cout << "Verifying IR...\n";
            if (Optimizer::verifyModule(codegen.getModule(), true)) {
                std::cout << "[OK] IR verification passed\n";
            }
        }

        // Print optimization statistics if requested
        if (opts.printOptStats) {
            Optimizer::printOptimizationStats(codegen.getModule());
        }
    } else {
        std::cout << "Compiling (no optimization)...\n";
    }

    // Emit optimized IR if requested
    if (opts.emitOptimizedIR) {
        String optIRFile = opts.outputFile + ".opt.ll";
        codegen.emitLLVMIR(optIRFile);
        std::cout << "Optimized IR: " << optIRFile << "\n";
    }

    if (!createDirectoryIfNeeded(opts.outputFile)) {
        return 1;
    }

    if (opts.emitLLVM) {
        String llFile = opts.outputFile + ".ll";
        codegen.emitLLVMIR(llFile);
        std::cout << "Generated: " << llFile << "\n";
        return 0;
    }

    if (opts.emitASM) {
        std::cout << "Assembly output not implemented\n";
        return 0;
    }

    String objFile = opts.outputFile + ".o";
    if (!codegen.compileToObject(objFile)) {
        std::cerr << "Failed to compile\n";
        return 1;
    }

    if (!linkExecutable(objFile, opts.outputFile, opts.optLevel)) {
        std::cerr << "Linking failed\n";
        return 1;
    }

#ifdef _WIN32
    std::cout << "Output: " << opts.outputFile << ".exe\n";
#else
    std::cout << "Output: " << opts.outputFile << "\n";
#endif

    return 0;
}
