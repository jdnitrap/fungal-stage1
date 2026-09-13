#include "builder.hpp"

namespace fungal {

LearningTemplate TemplateBuilder::build(
    const std::string& topic,
    const ConceptDictionary& dictionary) const {

    LearningTemplate tmpl;
    tmpl.topic = topic;

    std::vector<std::string> all_concepts = dictionary.get_all_concept_names();

    bool found = false;
    for (const auto& concept : all_concepts) {
        if (concept == topic) {
            found = true;
            break;
        }
    }

    if (!found) {
        tmpl.topic_recognized = false;
        tmpl.valid = false;
        return tmpl;
    }

    tmpl.topic_recognized = true;

    Concept concept = dictionary.get_concept(topic);
    tmpl.concepts.push_back(topic);

    if (concept.name.empty()) {
        tmpl.valid = false;
        return tmpl;
    }

    for (const auto& prereq : concept.prerequisites) {
        tmpl.prerequisites.push_back(prereq);
        tmpl.concepts.push_back(prereq);
    }

    tmpl.valid = true;
    return tmpl;
}

}  // namespace fungal
