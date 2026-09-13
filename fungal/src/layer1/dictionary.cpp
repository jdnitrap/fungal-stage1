#include "dictionary.hpp"

namespace fungal {

void ConceptDictionary::initialize_concepts() {
    concepts.clear();

    concepts["recursion"] = {
        "recursion",
        "A function that calls itself",
        {"base_case", "recursive_case"}
    };

    concepts["nesting_depth"] = {
        "nesting_depth",
        "Maximum depth of nested code blocks",
        {}
    };

    concepts["branch_count"] = {
        "branch_count",
        "Number of branching statements (if, while, for)",
        {}
    };

    concepts["function"] = {
        "function",
        "A callable unit of code",
        {}
    };

    concepts["base_case"] = {
        "base_case",
        "The termination condition for recursion",
        {}
    };

    concepts["recursive_case"] = {
        "recursive_case",
        "The recursive call within a function",
        {"function", "recursion"}
    };
}

Concept ConceptDictionary::get_concept(const std::string& name) const {
    auto it = concepts.find(name);
    if (it != concepts.end()) {
        return it->second;
    }
    return Concept{};
}

std::vector<std::string> ConceptDictionary::get_all_concept_names() const {
    std::vector<std::string> names;
    for (const auto& pair : concepts) {
        names.push_back(pair.first);
    }
    return names;
}

}  // namespace fungal
