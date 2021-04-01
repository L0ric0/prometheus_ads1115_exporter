#ifndef PROMETHEUS_ADS1115_EXPORTER_ADS1115_EXPORTER_HPP_
#define PROMETHEUS_ADS1115_EXPORTER_ADS1115_EXPORTER_HPP_

// prometheus_ads1115_exporter
#include "prometheus_ads1115_exporter/config.hpp"

// argparse
#include "argparse/argparse.hpp"

// stl
#include <array>
#include <string_view>

namespace ads1115_exporter
{
    class ADS1115Exporter
    {
      private:
        Config m_config;

      public:
        ADS1115Exporter(int argc, char* argv[]);
        static constexpr std::array<std::string_view, 2> config_file_paths {
            "config.yaml",
            "/etc/prometheus_ads1115_exporter/config.yaml"
        };

      private:
        Config generate_config(int argc, char* argv[]) const;
        argparse::ArgumentParser get_comandline_arguments(int argc, char* argv[]) const;
    };
} // namespace ads1115_exporter

#endif // PROMETHEUS_ADS1115_EXPORTER_ADS1115_EXPORTER_HPP_
