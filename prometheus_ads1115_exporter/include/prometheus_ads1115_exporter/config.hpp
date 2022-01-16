#ifndef PROMETHEUS_ADS1115_EXPORTER_CONFIG_HPP_
#define PROMETHEUS_ADS1115_EXPORTER_CONFIG_HPP_

// ads1115
#include "ads1115/config.hpp"
#include "ads1115/parameters.hpp"
#include "ads1115/threshold.hpp"

// argparse
#include "argparse/argparse.hpp"

// spdlog
#include "fmt/ranges.h"
#include "spdlog/spdlog.h"

// yaml-cpp
#include "yaml-cpp/yaml.h"

// stl
#include <string>
#include <unordered_map>
#include <vector>

namespace ads1115_exporter
{
    namespace
    {
        template <typename K, typename V>
        auto get_map_keys(const std::unordered_map<K, V>& map)
        {
            if constexpr (std::is_same_v<K, bool>) {
                std::vector<std::string> res;
                res.reserve(map.size());
                for (const auto& elem : map) {
                    if (elem.first) {
                        res.push_back("true");
                    } else {
                        res.push_back("false");
                    }
                }
                return res;
            } else {
                std::vector<K> res;
                res.reserve(map.size());
                for (const auto& elem : map) {
                    res.push_back(elem.first);
                }
                return res;
            }
        }
    } // namespace

    struct Device {
        std::string name;
        std::string path;
        ADS1115::ADDR address;
        ADS1115::Config config {};
        ADS1115::Threshold thresholds {};
        std::vector<ADS1115::MUX> inputs {};
    };

    struct Config {
        const std::string listen_address;
        const std::string port;

        const std::vector<Device> devices;

        Config(
            const argparse::ArgumentParser& comandline_arguments,
            const YAML::Node& configuration_file);

        static std::unordered_map<double, ADS1115::PGA> fullscale_pga_map()
        {
            return {
                { 6.144, ADS1115::PGA::FS_6_144 }, { 4.096, ADS1115::PGA::FS_4_096 },
                { 2.048, ADS1115::PGA::FS_2_048 }, { 1.024, ADS1115::PGA::FS_1_024 },
                { 0.512, ADS1115::PGA::FS_0_512 }, { 0.256, ADS1115::PGA::FS_0_256 },
            };
        };
        static std::unordered_map<int, ADS1115::DR> int_data_rate_map()
        {
            return {
                { 8, ADS1115::DR::SPS_8 },     { 16, ADS1115::DR::SPS_16 },
                { 32, ADS1115::DR::SPS_32 },   { 64, ADS1115::DR::SPS_64 },
                { 128, ADS1115::DR::SPS_128 }, { 250, ADS1115::DR::SPS_250 },
                { 475, ADS1115::DR::SPS_475 }, { 860, ADS1115::DR::SPS_860 },
            };
        };
        static std::unordered_map<int, ADS1115::COMP_QUE> int_comp_queue_map()
        {
            return {
                { 1, ADS1115::COMP_QUE::ONE_CONV },
                { 2, ADS1115::COMP_QUE::TWO_CONV },
                { 4, ADS1115::COMP_QUE::FOUR_CONV },
            };
        };
        static std::unordered_map<std::string, ADS1115::COMP_MODE> str_comp_mode_map()
        {
            return {
                { "traditional", ADS1115::COMP_MODE::TRAD_COMP },
                { "window", ADS1115::COMP_MODE::WINDOW_COMP },
            };
        };
        static std::unordered_map<std::string, ADS1115::COMP_POL> str_comp_pol_map()
        {
            return {
                { "low", ADS1115::COMP_POL::LOW },
                { "high", ADS1115::COMP_POL::HIGH },
            };
        };
        static std::unordered_map<bool, ADS1115::COMP_LAT> bool_comp_lat_map()
        {
            return {
                { false, ADS1115::COMP_LAT::NON_LATCHING },
                { true, ADS1115::COMP_LAT::LATCHING },
            };
        };
        static std::unordered_map<std::string, ADS1115::MUX> str_input_map()
        {
            return {
                { "AIN0_AIN1", ADS1115::MUX::AIN0_AIN1 }, { "AIN0_AIN3", ADS1115::MUX::AIN0_AIN3 },
                { "AIN1_AIN3", ADS1115::MUX::AIN1_AIN3 }, { "AIN2_AIN3", ADS1115::MUX::AIN2_AIN3 },
                { "AIN0_GND", ADS1115::MUX::AIN0_GND },   { "AIN1_GND", ADS1115::MUX::AIN1_GND },
                { "AIN2_GND", ADS1115::MUX::AIN2_GND },   { "AIN3_GND", ADS1115::MUX::AIN3_GND },
            };
        };
        static std::unordered_map<ADS1115::MUX, std::string> input_str_map()
        {
            return {
                { ADS1115::MUX::AIN0_AIN1, "AIN0_AIN1" }, { ADS1115::MUX::AIN0_AIN3, "AIN0_AIN3" },
                { ADS1115::MUX::AIN1_AIN3, "AIN1_AIN3" }, { ADS1115::MUX::AIN2_AIN3, "AIN2_AIN3" },
                { ADS1115::MUX::AIN0_GND, "AIN0_GND" },   { ADS1115::MUX::AIN1_GND, "AIN1_GND" },
                { ADS1115::MUX::AIN2_GND, "AIN2_GND" },   { ADS1115::MUX::AIN3_GND, "AIN3_GND" },
            };
        };
        static std::unordered_map<std::string, ADS1115::ADDR> str_address_map()
        {
            return {
                { "GND", ADS1115::ADDR::GND },
                { "VDD", ADS1115::ADDR::VDD },
                { "SDA", ADS1115::ADDR::SDA },
                { "SCL", ADS1115::ADDR::SCL },
            };
        };

      private:
        std::vector<Device> get_devices(const YAML::Node& config_devices) const;
        ADS1115::ADDR get_address(
            const YAML::Node& node,
            const std::string& key,
            const std::unordered_map<std::string, ADS1115::ADDR>& map,
            const std::string& default_value) const;
        ADS1115::Config get_comparator(const YAML::Node& node, ADS1115::Config device_config) const;

        template <typename TRead, typename TResult>
        TResult get_value(
            const YAML::Node& node,
            const std::string& key,
            const std::unordered_map<TRead, TResult>& map,
            const TRead& default_value) const
        {
            TRead value = node[key].as<TRead>(default_value);

            if (auto it = map.find(value); it != map.end()) {
                return it->second;
            }

            spdlog::critical("Illeagel value `{}` for key `{}` found.", value, key);
            spdlog::critical("Leagel values are: {}", get_map_keys(map));
            exit(1);
        }

        ADS1115::MUX get_input(
            const YAML::Node& node,
            const std::unordered_map<std::string, ADS1115::MUX>& map,
            const std::string& default_value) const
        {
            std::string value = node.as<std::string>(default_value);

            if (auto it = map.find(value); it != map.end()) {
                return it->second;
            }

            spdlog::critical("Illeagel value `{}` for `key inputs` found.", value);
            spdlog::critical("Leagel values are: {}", get_map_keys(map));
            exit(1);
        };
    };
} // namespace ads1115_exporter

#endif // PROMETHEUS_ADS1115_EXPORTER_CONFIG_HPP_
