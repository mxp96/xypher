#include "Common.h"
#include "XypherConfig.h"
#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "sema/SemanticAnalyzer.h"
#include "codegen/CodeGenerator.h"
#include "backend/Optimizer.h"
#include "ast/ASTDumper.h"
#include "frontend/Diagnostics.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <filesystem>

#ifdef _WIN32
    #include <windows.h>
    #ifndef MAX_PATH
        #define MAX_PATH 260
    #endif
#else
    #include <unistd.h>
    #include <sys/wait.h>
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
};

void printHelp() {
    std::cout << "Xypher Compiler (xypc) v" << XYPHER_VERSION_STRING << "\n\n";
    std::cout << "Usage: xypc [options] <input.xyp>\n\n";
    std::cout << "Options:\n";
    std::cout << "  -o <file>          Specify output file name\n";
    std::cout << "  --emit-llvm        Emit LLVM IR instead of binary\n";
    std::cout << "  --emit-asm         Emit assembly code\n";
    std::cout << "  --check-syntax     Check syntax only, don't compile\n";
    std::cout << "  --ast-dump         Dump AST and exit\n";
    std::cout << "  --debug            Enable debug mode\n";
    std::cout << "  -O<level>          Optimization level (0-3)\n";
    std::cout << "  -h, --help         Show this help message\n";
    std::cout << "  -v, --version      Show version information\n";
    std::cout << "\n";
    std::cout << "Examples:\n";
    std::cout << "  xypc main.xyp -o main\n";
    std::cout << "  xypc --emit-llvm program.xyp\n";
    std::cout << "  xypc --ast-dump test.xyp\n";
    std::cout << "  xypc -O2 main.xyp -o optimized\n";
}

void printVersion() {
    std::cout << "Xypher Compiler (xypc) v" << XYPHER_VERSION_STRING << "\n";
    std::cout << "A modern compiled programming language\n";
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
        } else if (arg == "--emit-asm") {
            opts.emitASM = true;
        } else if (arg == "--check-syntax") {
            opts.checkSyntaxOnly = true;
        } else if (arg == "--ast-dump") {
            opts.dumpAST = true;
        } else if (arg == "--debug") {
            opts.debugMode = true;
        } else if (arg.substr(0, 2) == "-O" && arg.length() == 3) {
            opts.optLevel = arg[2] - '0';
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
        exeDir,                           // Same directory
        exeDir / ".." / "bin",            // ../bin
        exeDir / ".." / ".." / "bin",     // ../../bin (for VS build)
        exeDir / "lib",                   // ./lib
        exeDir / ".." / "lib",            // ../lib
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

bool linkExecutable(const String& objFile, const String& exeFile) {
    // Auto-detect xystd library path based on compiler location
    fs::path stdLibPath = findStdLibPath();
    
#ifdef _WIN32
    String libName = "xystd.dll";
    String libPath = (stdLibPath / "xystd.lib").string();
    String dllPath = (stdLibPath / libName).string();
    
    // Check if library exists
    if (!fs::exists(dllPath)) {
        std::cerr << "Warning: Xypher standard library not found\n";
        std::cerr << "  Expected: " << dllPath << "\n";
        std::cerr << "  Compiler will use built-in printf instead\n";
        // Fallback: link without xystd
        String command = "clang " + objFile + " -o " + exeFile + ".exe";
        int result = system(command.c_str());
        return result == 0;
    }
    
    // Windows: Link with import library
    String command = "clang " + objFile + " -L\"" + stdLibPath.string() + "\" -lxystd -o " + exeFile + ".exe";
#else
    #ifdef __APPLE__
        String libName = "libxystd.dylib";
    #else
        String libName = "libxystd.so";
    #endif
    
    String libPath = (stdLibPath / libName).string();
    
    if (!fs::exists(libPath)) {
        std::cerr << "Warning: Xypher standard library not found\n";
        std::cerr << "  Expected: " << libPath << "\n";
        std::cerr << "  Compiler will use built-in printf instead\n";
        // Fallback: link without xystd
        String command = "clang " + objFile + " -o " + exeFile;
        int result = system(command.c_str());
        return result == 0;
    }
    
    // Unix: Link with rpath for runtime loading
    String command = "clang " + objFile + " -L\"" + stdLibPath.string() + "\" -lxystd -Wl,-rpath,\"" + stdLibPath.string() + "\" -o " + exeFile;
#endif
    
    int result = system(command.c_str());
    return result == 0;
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
        return 1;
    }
    
    DiagnosticEngine diags;
    Lexer lexer(source, opts.inputFile);
    Parser parser(lexer, diags);
    
    auto program = parser.parseProgram();
    
    if (diags.hasErrors()) {
        std::cerr << "Parsing failed with " << diags.getErrorCount() << " error(s)\n";
        return 1;
    }
    
    if (opts.checkSyntaxOnly) {
        std::cout << "Syntax check passed\n";
        return 0;
    }
    
    if (opts.dumpAST) {
        ASTDumper dumper;
        dumper.dump(program.get());
        return 0;
    }
    
    SemanticAnalyzer analyzer(diags);
    if (!analyzer.analyze(program.get())) {
        std::cerr << "Semantic analysis failed with " << diags.getErrorCount() << " error(s)\n";
        return 1;
    }
    
    CodeGenerator codegen(opts.inputFile, diags);
    if (!codegen.generate(program.get())) {
        std::cerr << "Code generation failed\n";
        return 1;
    }
    
    if (opts.optLevel > 0) {
        OptimizationLevel level = static_cast<OptimizationLevel>(opts.optLevel);
        Optimizer::optimize(codegen.getModule(), level);
    }
    
    // Create output directory if needed
    if (!createDirectoryIfNeeded(opts.outputFile)) {
        return 1;
    }
    
    if (opts.emitLLVM) {
        String llFile = opts.outputFile + ".ll";
        if (!createDirectoryIfNeeded(llFile)) {
            return 1;
        }
        codegen.emitLLVMIR(llFile);
        std::cout << "Generated LLVM IR: " << llFile << "\n";
        return 0;
    }
    
    if (opts.emitASM) {
        String asmFile = opts.outputFile + ".s";
        if (!createDirectoryIfNeeded(asmFile)) {
            return 1;
        }
        // Would emit assembly here
        std::cout << "Assembly output not yet implemented\n";
        return 0;
    }
    
    String objFile = opts.outputFile + ".o";
    if (!createDirectoryIfNeeded(objFile)) {
        return 1;
    }
    
    if (!codegen.compileToObject(objFile)) {
        std::cerr << "Failed to generate object file\n";
        return 1;
    }
    
    if (opts.debugMode) {
        std::cout << "Generated object file: " << objFile << "\n";
    }
    
    if (!linkExecutable(objFile, opts.outputFile)) {
        std::cerr << "Failed to link executable\n";
        std::cerr << "Note: Make sure clang is installed and in PATH\n";
        return 1;
    }
    
    if (opts.debugMode) {
#ifdef _WIN32
        std::cout << "Generated executable: " << opts.outputFile << ".exe\n";
#else
        std::cout << "Generated executable: " << opts.outputFile << "\n";
#endif
    } else {
        std::cout << "Compilation successful\n";
    }
    
    return 0;
}

