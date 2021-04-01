// prometheus_ads1115_exporter
#include "prometheus_ads1115_exporter/ads1115_exporter.hpp"

// ads1115
#include "ads1115/ads1115.hpp"
#include "ads1115/config.hpp"
#include "ads1115/parameters.hpp"

// prometheus
#include <prometheus/exposer.h>
#include <prometheus/gauge.h>
#include <prometheus/registry.h>

// spdlog
#include "spdlog/spdlog.h"

// stl
#include <chrono>
#include <memory>
#include <thread>

int main(int argc, char* argv[])
{
    ads1115_exporter::ADS1115Exporter exporter(argc, argv);

    prometheus::Exposer exposer { "0.0.0.0:33333" };
    auto registry = std::make_shared<prometheus::Registry>();
    auto& ads1115_readings
        = prometheus::BuildGauge().Name("ads1115").Help("ads1115 readings").Register(*registry);
    auto& ads1115_ain1_gnd = ads1115_readings.Add({ { "ads1115", "1" } });
    exposer.RegisterCollectable(registry);

    ADS1115::ADS1115 ads("/dev/i2c-1", ADS1115::ADDR::GND);
    ads.reset();
    ADS1115::Config conf {};
    conf.mux = ADS1115::MUX::AIN0_GND;
    conf.data_rate = ADS1115::DR::SPS_8;
    ads.setRegConfig(conf);

    for (;;) {
        ads1115_ain1_gnd.Set(ads.read());
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
