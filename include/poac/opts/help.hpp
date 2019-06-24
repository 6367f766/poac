#ifndef POAC_OPTS_HELP_HPP
#define POAC_OPTS_HELP_HPP

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <cstdlib>

#include "../core/except.hpp"
#include "../core/infer.hpp"
#include "../io/cli.hpp"
#include "../util/termcolor2.hpp"


// Forward-declaration
namespace poac::core::infer {
    template <typename S, typename OpTypeE, typename VS>
    auto _apply(S&& func, const OpTypeE& cmd, VS&& arg);
    template <typename S, typename VS>
    std::string apply(S&& func, S&& cmd, VS&& arg);

    extern const std::unordered_map<std::string, int> opts_map;
}

// TODO: help文を，コンパイル時に一つの文字列として変換する．

// TODO: optionではなく，helpコマンドとすれば，順序は，init helpを許されなくなるので明快になる．
// TODO: さらに，versionを，poacの部分に埋め込めば(もう一段階抽象化後)，optionを管理する必要がなくなる．
namespace poac::opts {
    namespace _help {
        template <typename S>
        void echo_option(S&& arg) {
            namespace except = core::except;
            using namespace std::string_literals;

            try {
                std::cout << "Usage: poac " << arg << " "
                          << core::infer::apply("options"s, std::forward<S>(arg), std::vector<S>())
                          << std::endl;
            }
            catch (const except::invalid_first_arg& e) {
                throw except::invalid_second_arg("--help");
            }
        }

        template<typename T, typename U>
        void show(const T& key, const U& value) {
            using namespace std::string_literals;
            // Eliminate -h and -v
            // It assumes two characters because the regular expression is slow.
            if (key[0] != '-') {
                std::cout << termcolor2::blue<> << termcolor2::bold<>
                          << "   " << std::setw(9) << std::left << key << "   "
                          << termcolor2::reset<>;

                std::cout << termcolor2::yellow<>
                          << core::infer::_apply("summary"s, value, std::vector<std::string>())
                          << termcolor2::reset<>
                          << std::endl;
            }
        }

        void exec_help() {
            std::cout << "Usage: poac <command> [<args>]" << std::endl << std::endl;

            std::cout << termcolor2::bold<>
                      << "Available commands:"
                      << termcolor2::reset<>
                      << std::endl;
            for (const auto& [name, value] : core::infer::opts_map) {
                show(name, value);
            }

            std::cout << std::endl
                      << "See `poac <command> --help` for information on a specific command.\n"
                         "For full documentation, see: https://github.com/poacpm/poac#readme\n";
        }

        template<typename VS>
        int _main(VS&& vs) {
            namespace except = core::except;
            if (vs.size() == 0) {
                exec_help();
                return EXIT_SUCCESS;
            }
            else if (vs.size() == 1) {
                echo_option(std::move(vs[0]));
                return EXIT_SUCCESS;
            }
            else {
                throw except::invalid_second_arg("--help");
            }
            // show only --help's option
        }
    }

    struct help {
        static std::string summary() {
            return "Display help for a command";
        }
        static std::string options() {
            return "<subcommad or option>";
        }

        template<typename VS>
        int operator()(VS&& argv) {
            return _help::_main(std::forward<VS>(argv));
        }
    };
} // end namespace
#endif // !POAC_OPTS_HELP_HPP
