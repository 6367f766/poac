#ifndef POAC_OPTS_CLEANUP_HPP
#define POAC_OPTS_CLEANUP_HPP

#include <iostream>
#include <string>
#include <cstdlib>

#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

#include "./install.hpp"
#include "./uninstall.hpp"
#include "../core/except.hpp"
#include "../core/name.hpp"
#include "poac/core/deper/resolve.hpp"
#include "../core/deper/lock.hpp"
#include "../io/yaml.hpp"
#include "../io/cli.hpp"


namespace poac::opts {
    namespace _cleanup {
        template<typename VS>
        int _main([[maybe_unused]] VS&& argv) {
            namespace yaml = io::yaml;
            namespace resolver = core::deper::resolve;
            namespace lock = core::deper::lock;
            namespace name = core::name;
            namespace fs = boost::filesystem;
            namespace cli = io::cli;


            // create resolved deps
            resolver::Resolved resolved_deps{};
            if (const auto locked_deps = lock::load()) {
                resolved_deps = *locked_deps;
            }
            else { // poac.lock does not exist
                const auto deps_node = yaml::load_config("deps").as<std::map<std::string, YAML::Node>>();
                const resolver::Deps deps = _install::resolve_packages(deps_node);
                resolved_deps = resolver::resolve(deps);
            }

            std::vector<std::string> package_names;
            for (const auto& [name, dep] : resolved_deps.backtracked) {
                const auto package_name = name::to_current(dep.source, name, dep.version);
                package_names.push_back(package_name);
            }

            // iterate directory
            const auto first = package_names.begin();
            const auto last = package_names.end();
            for (const auto& e : boost::make_iterator_range( fs::directory_iterator("deps"), {} )) {
                const auto found = std::find(first, last, e.path().filename().string());
                if (found == last) { // not found
                    fs::remove_all(e.path());
                    const auto info_state = "Remove unused package " + e.path().filename().string();
                    std::cout << cli::info << info_state << std::endl;
                }
            }

            return EXIT_SUCCESS;

            // TODO: cleanup _build directory
            // TODO: auto cleanup in install sub-command
        }

        void check_arguments(const std::vector<std::string>& argv) {
            namespace except = core::except;
            if (!argv.empty()) {
                throw except::invalid_second_arg("cleanup");
            }
        }
    }

    struct cleanup {
        static std::string summary() {
            return "Delete unnecessary things";
        }
        static std::string options() {
            return "<Nothing>";
        }
        template<typename VS>
        int operator()(VS&& argv) {
            _cleanup::check_arguments(argv);
            return _cleanup::_main(std::forward<VS>(argv));
        }
    };
} // end namespace
#endif // !POAC_OPTS_CLEANUP_HPP
