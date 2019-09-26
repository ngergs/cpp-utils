#include "timer.h"
#include <thread>

int main()
{
    TimeKeeper timeKeeper(false);
    auto exampleFunction = [](auto a) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        return 2 * a;
    };
    auto f = timeKeeper.wrap(exampleFunction, "Example");

    std::cout << std::to_string(f(2.5)) << std::endl;
    std::cout << std::to_string(f(2.5)) << std::endl;
    std::cout << std::to_string(f(2.5)) << std::endl;

    timeKeeper.getAverageTimings();
}
