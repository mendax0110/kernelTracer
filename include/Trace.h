#ifndef KERNELTRACER_TRACE_H
#define KERNELTRACER_TRACE_H

#include <string>
#include <utility>
#include <vector>
#include <regex>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstdint>

/// @brief Class to store a trace call.
class TraceCall
{
public:
    std::string module;
    std::string method;
    uint64_t calls;

    TraceCall(std::string  mod, std::string  meth, uint64_t callCount)
        : module(std::move(mod)), method(std::move(meth)), calls(callCount) {}
};

/// @brief Class to store a trace.
class Trace
{
public:

    /**
     * @brief Add a call to the trace.
     * @param call -> The call to add.
     */
    void addCall(const TraceCall& call);

    /**
     * @brief Sort the trace.
     */
    void sortTrace();

    /**
     * @brief Print the trace.
     */
    void printTrace() const;

    /**
     * @brief Check if the trace is empty.
     * @return -> True if the trace is empty, false otherwise.
     */
    bool empty();

private:
    std::vector<TraceCall> calls;

    /**
     * @brief Get the longest module name.
     * @return
     */
    [[nodiscard]] int longestModuleName() const;

    /**
     * @brief Get the longest method name.
     * @return
     */
    [[nodiscard]] int longestMethodName() const;

    /**
     * @brief Get the total number of calls.
     * @return
     */
    [[nodiscard]] uint64_t totalCalls() const;

    /**
     * @brief Get the maximum number of calls.
     * @return
     */
    [[nodiscard]] uint64_t maximumCalls() const;
};

#endif //KERNELTRACER_TRACE_H
