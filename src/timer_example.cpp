#include "timer.h"
#include "constructorReporter.h"
#include <thread>

int main()
{
    TimeKeeper<std::chrono::milliseconds> timeKeeperMilli(false);
    auto exampleFunction = [](auto a) -> auto
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        return 2 * a;
    };

    std::cout << std::to_string(timeKeeperMilli.wrap("Simple", exampleFunction, 2.5)) << std::endl;
    std::cout << std::to_string(timeKeeperMilli.wrap("Simple", exampleFunction, 2.5)) << std::endl;
    std::cout << std::to_string(timeKeeperMilli.wrap("Simple", exampleFunction, 2.5)) << std::endl;
    timeKeeperMilli.getAverageTimings();

    TimeKeeper<std::chrono::nanoseconds> timeKeeperNano(false);
    // Test if we forward move/copy correctly using swap-and-copy idiom
    auto forwardFunction = [](auto &&a) -> auto
    {
        return std::forward<decltype(a)>(a);
    };

    ConstructorReporter consReporter("constructTest", true);
    std::cout << "Test copy construct" << std::endl;
    auto consReporter2(timeKeeperNano.wrap("Forward", forwardFunction, consReporter));
    std::cout << "Test move construct" << std::endl;
    auto consReporter3(timeKeeperNano.wrap("Forward", forwardFunction, std::move(consReporter2)));
    std::cout << "Test copy assignment" << std::endl;
    consReporter3 = timeKeeperNano.wrap("Forward", forwardFunction, consReporter);
    std::cout << "Test move assignment" << std::endl;
    consReporter3 = timeKeeperNano.wrap("Forward", forwardFunction, std::move(consReporter));

    std::cout
        << "Loop tests" << std::endl;
    for (auto i = 0; i < 100000000; i++)
    {
        ConstructorReporter consReporterLoop("constructTest", false);
        timeKeeperNano.wrap("Copy construct", [&forwardFunction, &consReporterLoop]() { auto temp(forwardFunction(consReporterLoop)); });
        timeKeeperNano.wrap("Move construct", [&forwardFunction, &consReporterLoop]() { auto temp(forwardFunction(std::move(consReporterLoop))); });
        ConstructorReporter consReporterLoop2("constructTest", false);
        ConstructorReporter consReporterLoop3("constructTest", false);
        timeKeeperNano.wrap("Copy assign", [&forwardFunction, &consReporterLoop2, &consReporterLoop3]() { consReporterLoop3 = forwardFunction(consReporterLoop2); });
        timeKeeperNano.wrap("Move assign", [&forwardFunction, &consReporterLoop2, &consReporterLoop3]() { consReporterLoop3 = forwardFunction(std::move(consReporterLoop2)); });
    }

    timeKeeperNano.getAverageTimings();
}
