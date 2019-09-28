#include "timer.h"
#include "constructorReporter.h"
#include <thread>

int main()
{
    TimeKeeper timeKeeper(false);
    auto exampleFunction = [](auto a) -> auto
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        return 2 * a;
    };
    auto f = timeKeeper.wrap(exampleFunction, "Simple");

    std::cout << std::to_string(f(2.5)) << std::endl;
    std::cout << std::to_string(f(2.5)) << std::endl;
    std::cout << std::to_string(f(2.5)) << std::endl;

    // Test if we forward move/copy correctly using swap-and-copy idiom
    auto forwardFunction = [](auto &&a) -> auto
    {
        return std::forward<decltype(a)>(a);
    };
    auto f2 = timeKeeper.wrap(forwardFunction, "Forward");
    ConstructorReporter consReporter("constructTest");
    std::cout << "Test copy construct" << std::endl;
    auto consReporter2(f2(consReporter));
    std::cout << "Test move construct" << std::endl;
    auto consReporter3(f2(std::move(consReporter2)));
    std::cout << "Test copy assignment" << std::endl;
    consReporter3 = f2(consReporter);
    std::cout << "Test move assignment" << std::endl;
    consReporter3 = f2(std::move(consReporter));

    timeKeeper.getAverageTimings();
}
