find_package(spdlog REQUIRED)
find_package(prometheus-cpp REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(argparse REQUIRED)

add_executable(prometheus-ads1115-exporter
    "include/prometheus_ads1115_exporter/config.hpp" "src/config.cpp"
    "include/prometheus_ads1115_exporter/ads1115_exporter.hpp" "src/ads1115_exporter.cpp"
    "src/main.cpp"
)

target_include_directories(prometheus-ads1115-exporter PUBLIC include)

target_link_libraries(prometheus-ads1115-exporter PUBLIC ads1115::ads1115)
target_link_libraries(prometheus-ads1115-exporter PRIVATE argparse::argparse)
target_link_libraries(prometheus-ads1115-exporter PUBLIC spdlog::spdlog)
target_link_libraries(prometheus-ads1115-exporter PUBLIC prometheus-cpp::pull)
target_link_libraries(prometheus-ads1115-exporter PUBLIC yaml-cpp)

add_library(prometheus-ads1115-exporter_lib EXCLUDE_FROM_ALL
    "include/prometheus_ads1115_exporter/config.hpp" "src/config.cpp"
    "include/prometheus_ads1115_exporter/ads1115_exporter.hpp" "src/ads1115_exporter.cpp"
    "src/main.cpp"
)

target_include_directories(prometheus-ads1115-exporter_lib PUBLIC include)

target_link_libraries(prometheus-ads1115-exporter_lib PUBLIC ads1115::ads1115)
target_link_libraries(prometheus-ads1115-exporter_lib PRIVATE argparse::argparse)
target_link_libraries(prometheus-ads1115-exporter_lib PUBLIC spdlog::spdlog)
target_link_libraries(prometheus-ads1115-exporter_lib PUBLIC prometheus-cpp::pull)
target_link_libraries(prometheus-ads1115-exporter_lib PUBLIC yaml-cpp)
