"""Code analysis strategies for bug detection."""

from abc import ABC, abstractmethod


class Strategy(ABC):
    """Abstract base for code analysis strategies."""

    @abstractmethod
    def apply(self, code_snippet: str) -> bool:
        """Analyze code and return boolean claim (has bug or not)."""
        pass


class PatternMatcherStrategy(Strategy):
    """Weak pattern-matching heuristics for C++ bugs.

    Note: This is intentionally weak. The learning signal comes from
    oracle feedback, not from strategy strength. Improvements possible
    once oracle is trustworthy.
    """

    def apply(self, code_snippet: str) -> bool:
        """Return True if code looks like it has a bug."""
        # Check for common C++ bug patterns via simple string matching
        patterns_suggesting_bugs = [
            "->",  # dereference without guard
            "int x;",  # uninitialized variable (heuristic)
            "strcpy",  # unsafe string copy
            "sprintf",  # unsafe format
            "gets",  # unsafe input
            "delete",  # potential use-after-free
            "new ",  # potential memory leak
        ]

        for pattern in patterns_suggesting_bugs:
            if pattern in code_snippet:
                return True

        return False
