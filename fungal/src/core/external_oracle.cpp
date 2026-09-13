#include "core/external_oracle.hpp"
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <unistd.h>

namespace fungal::core {

ExternalOracle::ExternalOracle() {
    // Verify clang++ exists and is callable
    int ret = system("which clang++ > /dev/null 2>&1");
    if (ret != 0) {
        throw std::runtime_error("ExternalOracle: clang++ not found in PATH. "
                                 "Install clang++ or add it to PATH.");
    }
}

std::string ExternalOracle::wrap_in_cpp(const std::string& code_snippet) const {
    std::ostringstream cpp;
    cpp << "#include <iostream>\n";
    cpp << "#include <cstring>\n";
    cpp << "#include <climits>\n";
    cpp << "#include <new>\n";
    cpp << "\n";
    cpp << "int main() {\n";
    cpp << "    " << code_snippet << "\n";
    cpp << "    return 0;\n";
    cpp << "}\n";
    return cpp.str();
}

bool ExternalOracle::compile_and_check(const std::string& code_snippet, std::string& compiler_output) {
    // Wrap code in valid C++
    std::string cpp_code = wrap_in_cpp(code_snippet);

    // Create unique temp file for this invocation (thread-safe, no race condition)
    char temp_src[] = "/tmp/fungal_oracle_src_XXXXXX";
    char temp_exe[] = "/tmp/fungal_oracle_exe_XXXXXX";
    char temp_log[] = "/tmp/fungal_oracle_log_XXXXXX";

    int fd_src = mkstemp(temp_src);
    int fd_exe = mkstemp(temp_exe);
    int fd_log = mkstemp(temp_log);
    if (fd_src < 0 || fd_exe < 0 || fd_log < 0) {
        compiler_output = "Error: could not create temp files";
        if (fd_src >= 0) ::close(fd_src);
        if (fd_exe >= 0) ::close(fd_exe);
        if (fd_log >= 0) ::close(fd_log);
        return false;
    }
    ::close(fd_src);
    ::close(fd_exe);
    ::close(fd_log);

    // Write source code to unique temp file
    std::ofstream temp_file(temp_src);
    if (!temp_file) {
        compiler_output = "Error: could not write temp source file";
        std::remove(temp_src);
        std::remove(temp_exe);
        std::remove(temp_log);
        return false;
    }
    temp_file << cpp_code;
    temp_file.close();

    // Compile with clang++ and capture output (with 5-second timeout)
    // Use -fsanitize=undefined to catch UB at compile time
    // Use 'timeout' command to enforce max execution time
    std::string cmd = std::string("timeout 5 clang++ -Wall -Wextra -std=c++17 -fsanitize=undefined ")
                      + temp_src + " -o " + temp_exe + " > " + temp_log + " 2>&1";
    int result = system(cmd.c_str());

    // Read compilation output from log file
    std::ifstream log_file(temp_log);
    if (!log_file) {
        compiler_output = "Error: could not read compiler output log";
        std::remove(temp_src);
        std::remove(temp_exe);
        std::remove(temp_log);
        return (result != 0);
    }

    std::stringstream buffer;
    buffer << log_file.rdbuf();
    compiler_output = buffer.str();
    log_file.close();

    // Compilation succeeded (exit code 0) means no errors
    // Non-zero exit means errors detected (includes timeout exit code 124)
    // Also check for warnings which indicate potential bugs
    bool has_issues = (result != 0) ||
                      compiler_output.find("warning:") != std::string::npos ||
                      compiler_output.find("error:") != std::string::npos;

    // Cleanup all temp files (RAII pattern via scope exit)
    std::remove(temp_src);
    std::remove(temp_log);
    std::remove(temp_exe);

    return has_issues;
}

bool ExternalOracle::has_bug(const std::string& code_snippet) {
    std::string compiler_output;
    return compile_and_check(code_snippet, compiler_output);
}

}  // namespace fungal::core
