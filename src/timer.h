#ifndef _TIMER_H_
#define _TIMER_H_

#include <chrono>
#include <iostream>
#include <mutex>
#include <unordered_map>
#include <vector>

template <class Duration>
class TimeKeeper
{
private:
    const bool threadsafe;
    std::mutex mutex;
    std::unordered_map<std::string, std::vector<int64_t>> timings;
    void pushTiming(std::string identifier, int64_t timing)
    {
        if (threadsafe)
        {
            std::unique_lock<std::mutex> lock(mutex);
            timings[identifier].push_back(timing);
        }
        else
        {
            timings[identifier].push_back(timing);
        }
    }

public:
    TimeKeeper(TimeKeeper &other) = delete;
    TimeKeeper(TimeKeeper &&other) = default;
    TimeKeeper &operator=(TimeKeeper &other) = delete;
    TimeKeeper &operator=(TimeKeeper &&other) = default;
    TimeKeeper(bool threadsafe) : threadsafe(threadsafe){};
    template <typename Identifier, typename Func, typename... Args,
              typename std::enable_if<!std::is_void<std::result_of_t<Func(Args...)>>::value>::type * = nullptr>
    auto wrap(Identifier &&identifier, Func &&func, Args &&... args)
    {
        auto start = std::chrono::steady_clock::now();
        auto result = func(std::forward<decltype(args)>(args)...);
        pushTiming(identifier, std::chrono::duration_cast<Duration>(
                                   std::chrono::steady_clock::now() - start)
                                   .count());
        return std::forward<decltype(result)>(result);
    }
    template <typename Identifier, typename Func, typename... Args,
              typename std::enable_if<std::is_void<std::result_of_t<Func(Args...)>>::value>::type * = nullptr>
    void wrap(Identifier &&identifier, Func &&func, Args &&... args)
    {
        auto start = std::chrono::steady_clock::now();
        func(std::forward<decltype(args)>(args)...);
        pushTiming(identifier, std::chrono::duration_cast<Duration>(
                                   std::chrono::steady_clock::now() - start)
                                   .count());
    }
    void getAverageTimings(std::ostream &out = std::cout) const
    {
        for (const auto &timing : timings)
        {
            out << timing.first << ": ";
            double average = 0.0;
            for (const auto time : timing.second)
            {
                average += time;
            }
            out << average / timing.second.size() << std::endl;
        }
    }
};
#endif
