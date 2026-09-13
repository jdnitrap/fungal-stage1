#include "production/all_production.hpp"
#include <iostream>
#include <csignal>
#include <atomic>

using namespace fungal::production;

std::atomic<bool> shutdown_requested{false};

void signal_handler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        shutdown_requested = true;
    }
}

int main(int argc, char* argv[]) {
    try {
        std::string config_file = "config.yaml";

        // Parse command-line arguments
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "-c" || arg == "--config") {
                if (i + 1 < argc) {
                    config_file = argv[++i];
                }
            } else if (arg == "-h" || arg == "--help") {
                std::cout << "Fungal Production Server\n"
                          << "Usage: " << argv[0] << " [options]\n"
                          << "Options:\n"
                          << "  -c, --config FILE    Configuration file (default: config.yaml)\n"
                          << "  -h, --help           Show this help message\n";
                return 0;
            }
        }

        // Register signal handlers
        std::signal(SIGINT, signal_handler);
        std::signal(SIGTERM, signal_handler);

        // Initialize production system
        auto& system = ProductionSystem::instance();
        system.initialize(config_file);

        // Start the system
        system.start();

        auto& logger = system.logger();
        logger.info("Production server running. Press Ctrl+C to shutdown.");

        // Main event loop
        while (!shutdown_requested) {
            std::this_thread::sleep_for(std::chrono::seconds(1));

            // Periodic health checks
            auto health_status = system.health().get_full_health_status();
            if (health_status["overall"] != "healthy") {
                logger.warn("System health degraded");
            }
        }

        logger.info("Shutdown requested");

        // Graceful shutdown
        system.stop();

        std::cout << "Shutdown complete\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}
