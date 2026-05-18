#include "Logger.hpp"
#include <iostream>
#include <thread>
#include <vector>
#include <cstdlib>

int main() {
    // Test 1: Basic console logging (internal mutex)
    {
        Logger log("TestLogger");
        log.logMessage("Starting test");
        log.logSuccess("All good");
        log.logWarning("Something might be off");
        log.logError("Critical failure");
    }

    std::cout << "\n--- File logging test ---\n" << std::endl;

    // Test 2: File logging
    {
        Logger log("FileLogger");
        log.enableFileLogging("logs/test_log.txt");

        log.logMessage("This goes to file and console");
        log.logSuccess("Success logged");
        log.logWarning("Warning logged");
        log.logError("Error logged");

        log.disableFileLogging();
    }

    std::cout << "\n--- Multi-thread test ---\n" << std::endl;

    // Test 3: Shared console mutex across threads
    std::mutex sharedMutex;
    std::vector<std::thread> threads;

    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([&sharedMutex, i]() {
            Logger log("Thread" + std::to_string(i), sharedMutex);
            log.logMessage("Hello from thread " + std::to_string(i));
            log.logSuccess("Thread success");
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "\nAll tests completed.\n";
    system("pause");
    return 0;
}