// prometheus_ads1116_exporter
#include "prometheus_ads1115_exporter/config.hpp"

// ads1115
#include "ads1115/parameters.hpp"

// argparse
#include "argparse/argparse.hpp"

// spdlog
#include "fmt/ranges.h"
#include "spdlog/spdlog.h"

// yaml-cpp
#include "yaml-cpp/yaml.h"

// stl
#include <cstdint>
#include <string>
#include <vector>

namespace ads1115_exporter
{

    Config::Config(
        const argparse::ArgumentParser& comandline_arguments,
        const YAML::Node& configuration_file)
        : m_listen_address(configuration_file["listen_address"].as<std::string>()),
          m_port(configuration_file["port"].as<std::string>()),
          m_devices(get_devices(configuration_file["devices"]))
    {
        spdlog::info("Config read.");
    };

    ADS1115::Config
    Config::get_comparator(const YAML::Node& node, ADS1115::Config device_config) const
    {
        if (!node["enabled"].as<bool>(false)) {
            device_config.comp_que = ADS1115::COMP_QUE::DISABLE_COMP;
            return device_config;
        }

        device_config.comp_que = get_value(node, "queue", int_comp_queue_map(), 1);
        device_config.comp_mode = get_value<std::string, ADS1115::COMP_MODE>(
            node,
            "mode",
            str_comp_mode_map(),
            "traditional");
        device_config.comp_pol = get_value<std::string, ADS1115::COMP_POL>(
            node,
            "polarity",
            str_comp_pol_map(),
            "low");
        device_config.comp_lat = get_value(node, "latching", bool_comp_lat_map(), false);

        return device_config;
    }

    ADS1115::ADDR Config::get_address(
        const YAML::Node& node,
        const std::string& key,
        const std::unordered_map<std::string, ADS1115::ADDR>& map,
        const std::string& default_value) const
    {
        try {
            uint16_t value = node[key].as<uint16_t>();
            return static_cast<ADS1115::ADDR>(value);
        } catch (const YAML::BadConversion& err) {
            std::string value = node[key].as<std::string>("GND");
            if (auto it = map.find(value); it != map.end()) {
                return it->second;
            }
            spdlog::critical("Illeagel value `{}` for key `{}` found.", value, key);
            spdlog::critical("Leagel values are: {}", get_map_keys(map));
            exit(1);
        }
    }

    std::vector<Device> Config::get_devices(const YAML::Node& config_devices) const
    {
        std::vector<Device> devices;
        devices.reserve(config_devices.size());
        for (const auto& elem : config_devices) {
            Device device {};
            device.name = elem["name"].as<std::string>("ads1115");
            device.path = elem["device_path"].as<std::string>("/dev/i2c-1");
            device.address = get_address(elem, "device_address", str_address_map(), "GND");
            device.config.pga
                = get_value(elem, "programmable_gain_amplifier", fullscale_pga_map(), 2.048);
            device.config.data_rate = get_value(elem, "data_rate", int_data_rate_map(), 128);

            device.config = get_comparator(elem["comparator"], device.config);

            device.thresholds = ADS1115::Threshold(
                elem["comparator"]["thresholds"]["low"].as<uint16_t>(-32768),
                elem["comparator"]["thresholds"]["high"].as<uint16_t>(32767));

            device.inputs.reserve(elem["inputs"].size());
            for (const auto& input : elem["inputs"]) {
                device.inputs.push_back(get_input(input, str_input_map(), "AIN0_AIN1"));
            }
            devices.push_back(device);
        }
        return devices;
    }
} // namespace ads1115_exporter
