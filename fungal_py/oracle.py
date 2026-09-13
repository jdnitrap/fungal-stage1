"""Oracles for ground truth bug detection."""

import subprocess
import tempfile
import os
import shutil
from abc import ABC, abstractmethod


class Oracle(ABC):
    """Abstract base for bug detection oracles."""

    @abstractmethod
    def has_bug(self, code_snippet: str) -> bool:
        """Return True if code has a bug, False if clean."""
        pass


class ExternalOracle(Oracle):
    """Real C++ compilation as ground truth.

    Uses clang++ with warnings and sanitizers to detect actual bugs.
    This is the trustworthy teacher for v1 learning.

    Label policy:
    - BUG (True): compilation error, or warning from -Wall -Wextra
    - OK (False): clean compilation with no errors or warnings
    """

    def __init__(self, compiler: str = "clang++", timeout_sec: float = 5.0):
        self.compiler = compiler
        self.timeout_sec = timeout_sec
        self._verify_compiler()

    def _verify_compiler(self):
        """Ensure compiler exists and is available."""
        try:
            result = subprocess.run(
                [self.compiler, "--version"],
                capture_output=True,
                timeout=2.0,
                check=False
            )
            if result.returncode != 0:
                raise RuntimeError(f"{self.compiler} not found or not working")
        except (FileNotFoundError, subprocess.TimeoutExpired):
            raise RuntimeError(f"Compiler {self.compiler} not available")

    def _wrap_in_cpp(self, code_snippet: str) -> str:
        """Wrap raw code in valid C++ with includes."""
        return f"""#include <iostream>
#include <cstring>
#include <climits>
#include <new>

int main() {{
    {code_snippet}
    return 0;
}}
"""

    def has_bug(self, code_snippet: str) -> bool:
        """Check code via real compilation."""
        cpp_code = self._wrap_in_cpp(code_snippet)

        # Create temp file with unique name
        temp_dir = tempfile.gettempdir()
        with tempfile.NamedTemporaryFile(
            mode='w',
            suffix='.cpp',
            dir=temp_dir,
            delete=False
        ) as f:
            temp_cpp = f.name
            f.write(cpp_code)

        try:
            # Compile with strict warnings
            result = subprocess.run(
                [
                    self.compiler,
                    "-Wall", "-Wextra",
                    "-std=c++17",
                    "-fsanitize=undefined",
                    temp_cpp,
                    "-o", os.devnull
                ],
                capture_output=True,
                timeout=self.timeout_sec,
                text=True
            )

            # Any compilation error or warning indicates a bug
            has_error = result.returncode != 0
            has_warning = "warning:" in result.stderr or "warning:" in result.stdout

            return has_error or has_warning

        except subprocess.TimeoutExpired:
            # Timeout is treated as potential bug (infinite loop, hang)
            return True
        except Exception as e:
            # Unexpected error: treat conservatively as bug
            print(f"Warning: oracle check failed: {e}")
            return True
        finally:
            # Clean up temp file
            try:
                os.unlink(temp_cpp)
            except:
                pass
