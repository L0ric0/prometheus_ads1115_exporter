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

    exporter.run();
}
