#ifndef PROMETHEUS_ADS1115_EXPORTER_ADS1115_EXPORTER_HPP_
#define PROMETHEUS_ADS1115_EXPORTER_ADS1115_EXPORTER_HPP_

// prometheus_ads1115_exporter
#include "prometheus_ads1115_exporter/config.hpp"

// ads1115
#include "ads1115/ads1115.hpp"

// argparse
#include "argparse/argparse.hpp"

// stl
#include <array>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

namespace ads1115_exporter
{
    class ADS1115Exporter
    {
      private:
        struct Device {
            ADS1115::ADS1115 device;
            std::string name;
            std::vector<ADS1115::MUX> inputs;
        };
        std::string m_listen_address;
        std::string m_port;
        std::vector<Device> m_devices;

      public:
        ADS1115Exporter(int argc, char* argv[]);
        void run();
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
