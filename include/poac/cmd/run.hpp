#ifndef POAC_CMD_RUN_HPP_
#define POAC_CMD_RUN_HPP_

// external
#include <spdlog/spdlog.h> // NOLINT(build/include_order)
#include <structopt/app.hpp>
#include <toml.hpp>

// internal
#include <poac/cmd/build.hpp>
#include <poac/core/validator.hpp>
#include <poac/poac.hpp>
#include <poac/util/shell.hpp>

namespace poac::cmd::run {

using FailedToRun = Error<"`{}` finished with return code 1", fs::path>;

struct Options : structopt::sub_command {
  /// Build artifacts in release mode, with optimizations
  Option<bool> release = false;
};

[[nodiscard]] Result<void>
exec(const Options& opts) {
  spdlog::trace("Checking if required config exists ...");
  Try(core::validator::required_config_exists().map_err(to_anyhow));

  spdlog::trace("Parsing the manifest file ...");
  // TODO(ken-matsui): parse as a static type rather than toml::value
  const toml::value manifest = toml::parse(data::manifest::name);
  const String name = toml::find<String>(manifest, "package", "name");

  const Option<fs::path> output = Try(
      build::build({.release = opts.release}, manifest).with_context([&name] {
        return Err<build::FailedToBuild>(name).get();
      })
  );
  if (!output.has_value()) {
    return Ok();
  }

  const fs::path executable = output.value() / name;
  spdlog::info("{:>25} `{}`", "Running"_bold_green, executable);
  if (!util::shell::Cmd(executable).exec_no_capture()) {
    return Err<FailedToRun>(executable);
  }
  return Ok();
}

} // namespace poac::cmd::run

STRUCTOPT(poac::cmd::run::Options, release);

#endif // POAC_CMD_RUN_HPP_