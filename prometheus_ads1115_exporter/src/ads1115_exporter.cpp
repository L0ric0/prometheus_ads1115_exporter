// prometheus_ads1115_exporter
#include "prometheus_ads1115_exporter/ads1115_exporter.hpp"

#include "prometheus_ads1115_exporter/config.hpp"

// ads1115
#include "ads1115/ads1115.hpp"
#include "ads1115/config.hpp"

// argparse
#include "argparse/argparse.hpp"

// prometheus
#include <prometheus/exposer.h>
#include <prometheus/gauge.h>
#include <prometheus/registry.h>

// spdlog
#include "spdlog/spdlog.h"

// stl
#include <filesystem>
#include <memory>
#include <thread>

namespace ads1115_exporter
{
    ADS1115Exporter::ADS1115Exporter(int argc, char* argv[])
    {
        spdlog::info("Initializing ADS1115Exporter");
        const auto config = generate_config(argc, argv);
        m_listen_address = config.listen_address;
        m_port = config.port;
        std::vector<::ads1115_exporter::Device> conf_devices = config.devices;

        m_devices.reserve(conf_devices.size());
        for (const auto& device_conf : conf_devices) {
            ADS1115::ADS1115 device { device_conf.path, device_conf.address };
            device.setRegConfig(device_conf.config);
            device.setRegThreshold(device_conf.thresholds);
            m_devices.emplace_back(std::move(device), device_conf.name, device_conf.inputs);
        }
    }

    void ADS1115Exporter::run()
    {
        spdlog::info("Initalizing prometheus.");
        prometheus::Exposer exposer { m_listen_address + ":" + m_port };
        auto registry = std::make_shared<prometheus::Registry>();
        auto& ads1115_readings = prometheus::BuildGauge()
                                     .Name("ads1115_measurement_volts")
                                     .Help("ads1115 readings")
                                     .Register(*registry);
        exposer.RegisterCollectable(registry);

        spdlog::info("Running data collection loop.");
        for (;;) {
            for (auto& device : m_devices) {
                for (const auto& input : device.inputs) {
                    auto& input_readings
                        = ads1115_readings.Add({ { "device", device.name },
                                                 { "input", Config::input_str_map().at(input) } });

                    auto device_conf = device.device.getRegConfig();
                    if (device_conf.mux != input) {
                        device_conf.mux = input;
                        device.device.setRegConfig(device_conf);
                    };

                    input_readings.Set(device.device.readVoltage());
                }
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    argparse::ArgumentParser ADS1115Exporter::get_comandline_arguments(int argc, char* argv[]) const
    {
        spdlog::info("Parsing comandline arguments.");
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
