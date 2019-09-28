#ifndef _CONSTRUCTOR_REPORTER_H_
#define _CONSTRUCTOR_REPORTER_H_
#include <iostream>

class ConstructorReporter
{
private:
    std::string identifier;
    bool verbose;
    ConstructorReporter() {}

public:
    friend void swap(ConstructorReporter &first, ConstructorReporter &second) noexcept
    {
        // enable ADL
        using std::swap;
        // by swapping the members of two objects,
        // the two objects are effectively swapped
        swap(first.identifier, second.identifier);
    }
    ConstructorReporter(std::string identifier, bool verbose) : identifier(identifier), verbose(verbose)
    {
        if (verbose)
            std::cout << identifier << ": Initial constructor." << std::endl;
    }
    ConstructorReporter(const ConstructorReporter &other) : identifier(other.identifier), verbose(other.verbose)
    {
        if (verbose)
            std::cout << identifier << ": Copy constructor." << std::endl;
    }
    ConstructorReporter(ConstructorReporter &&other) : identifier(std::move(other.identifier)), verbose(other.verbose)
    {
        if (verbose)
            std::cout << identifier << ": Move constructor." << std::endl;
    }
    ConstructorReporter &operator=(ConstructorReporter other)
    {
        if (verbose)
            std::cout << identifier << ": Swap assignment." << std::endl;
        // swap-and-copy idiom
        swap(*this, other);
        return *this;
    }
};
#endif