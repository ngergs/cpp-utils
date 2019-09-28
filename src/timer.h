#ifndef _TIMER_H_
#define _TIMER_H_

#include <chrono>
#include <iostream>
#include <mutex>
#include <unordered_map>
#include <vector>

class TimeKeeper
{
private:
    const bool threadsafe;
    std::mutex mutex;
    std::unordered_map<std::string, std::vector<int64_t>> timings;

public:
    TimeKeeper(TimeKeeper &other) = delete;
    TimeKeeper(TimeKeeper &&other) = default;
    TimeKeeper &operator=(TimeKeeper &other) = delete;
    TimeKeeper &operator=(TimeKeeper &&other) = default;
    TimeKeeper(bool threadsafe) : threadsafe(threadsafe){};
    template <typename Func>
    auto wrap(Func &&func, std::string identifier)
    {
        return [ threadsafe = this->threadsafe,
                 &mutex = this->mutex,
                 &timings = this->timings,
                 func = std::forward<Func>(func),
                 identifier = std::move(identifier) ](auto &&... args) -> auto
        {
            auto start = std::chrono::steady_clock::now();
            decltype(auto) result = func(std::forward<decltype(args)>(args)...);
            auto timing = std::chrono::duration_cast<std::chrono::milliseconds>(
                              std::chrono::steady_clock::now() - start)
                              .count();
            if (threadsafe)
            {
                std::unique_lock<std::mutex> lock(mutex);
                timings[identifier].push_back(timing);
            }
            else
            {
                timings[identifier].push_back(timing);
            }
            return result;
        };
    }
    void getAverageTimings(std::ostream &out = std::cout)
    {
        for (const auto &timing : timings)
        {
            out << timing.first << ": ";
            double average = 0.0;
            for (const auto time : timing.second)
            {
                average += time;
            }
            out << average / timing.second.size() << "ms" << std::endl;
        }
    }
};
#endif
