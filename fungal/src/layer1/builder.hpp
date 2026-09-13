#pragma once
#include "../../include/types.hpp"
#include "dictionary.hpp"
#include <string>

namespace fungal {

class TemplateBuilder {
public:
    LearningTemplate build(
        const std::string& topic,
        const ConceptDictionary& dictionary) const;
};

}  // namespace fungal
