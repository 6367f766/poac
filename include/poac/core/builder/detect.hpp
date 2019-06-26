#ifndef POAC_CORE_BUILDER_DETECT_HPP
#define POAC_CORE_BUILDER_DETECT_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include <boost/algorithm/string.hpp>

#include "../except.hpp"
#include "../../io/yaml.hpp"
#include "../../util/shell.hpp"

namespace poac::core::builder::detect {
    std::string check_support_build_system(const std::string& system) {
        if (system != "poac" && system != "cmake") {
            throw except::error("Unknown build system ", system);
        }
        return system;
    }

    std::optional<std::string>
    build_system(const YAML::Node& node)
    {
        namespace yaml = io::yaml;

        if (const auto system = yaml::get<std::string>(node, "build")) {
            return check_support_build_system(*system);
        }
        else if (const auto build_node = yaml::get<std::map<std::string, YAML::Node>>(node, "build")) {
            YAML::Node build_node2;
            try {
                build_node2 = (*build_node).at("system");
            }
            catch(std::out_of_range&) {
                return std::nullopt;
            }

            if (const auto system2 = yaml::get<std::string>(build_node2)) {
                return check_support_build_system(*system2);
            }
        }
        // No build required
        return std::nullopt;
    }
} // end namespace
#endif // POAC_CORE_BUILDER_DETECT_HPP
