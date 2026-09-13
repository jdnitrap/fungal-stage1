#pragma once
#include "../../include/types.hpp"
#include <string>
#include <map>
#include <vector>

namespace fungal {

struct Concept {
    std::string name;
    std::string definition;
    std::vector<std::string> prerequisites;
};

class ConceptDictionary {
public:
    void initialize_concepts();
    Concept get_concept(const std::string& name) const;
    std::vector<std::string> get_all_concept_names() const;

private:
    std::map<std::string, Concept> concepts;
};

}  // namespace fungal
