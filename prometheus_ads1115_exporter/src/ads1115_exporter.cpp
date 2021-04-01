// prometheus_ads1115_exporter
#include "prometheus_ads1115_exporter/ads1115_exporter.hpp"

#include "prometheus_ads1115_exporter/config.hpp"

// argparse
#include "argparse/argparse.hpp"

// fmt
#include "fmt/ranges.h"

// spdlog
#include "spdlog/fmt/ostr.h"
#include "spdlog/spdlog.h"

// yaml-cpp
#include "yaml-cpp/yaml.h"

// stl
#include <filesystem>

namespace ads1115_exporter
{
    ADS1115Exporter::ADS1115Exporter(int argc, char* argv[]) : m_config(generate_config(argc, argv))
    {
    }

    argparse::ArgumentParser ADS1115Exporter::get_comandline_arguments(int argc, char* argv[]) const
    {
        argparse::ArgumentParser parser("Prometheus ADS1113/4/5 exporter");
        parser.add_argument("-c", "--config").help("Path to the cofiguration file.");
        parser.add_argument("-h", "--help")
            .help("Print this help message and exit.")
            .default_value(false)
            .implicit_value(true);
        parser.add_argument("-v", "--verbose")
            .help("Enable verbose logging.")
            .default_value(false)
            .implicit_value(true);

        try {
            parser.parse_args(argc, argv);
        } catch (const std::runtime_error& err) {
            spdlog::critical("{}", err.what());
            spdlog::info("{}", parser.help().str());
            exit(1);
        }

        if (parser["--help"] == true) {
            spdlog::info("{}", parser.help().str());
            exit(0);
        }

        if (parser["--verbose"] == true) {
            spdlog::set_level(spdlog::level::trace);
        }

        return parser;
    }

    Config ADS1115Exporter::generate_config(int argc, char* argv[]) const
    {
        argparse::ArgumentParser parser = get_comandline_arguments(argc, argv);

        std::filesystem::path config_file;
        if (auto fn = parser.present("--config")) {
            config_file = std::filesystem::path(*fn);
        } else {
            for (const auto& elem : config_file_paths) {
                spdlog::trace("Trying configfile `{}`", elem);
                if (std::filesystem::exists(elem)) {
                    config_file = elem;
                }
            }
        }

        try {
            YAML::Node configuration = YAML::LoadFile(config_file);
            return Config(parser, configuration);
        } catch (const YAML::BadFile& err) {
            spdlog::critical("Unable to read configuration file.");
            if (auto fn = parser.present("--config")) {
                spdlog::critical("Unable to open `{}`.", *fn);
            } else {
                spdlog::critical("Searched for {}", config_file_paths);
            }
            exit(1);
        }
    }
} // namespace ads1115_exporter
