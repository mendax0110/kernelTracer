#include <numeric>
#include "../include/Trace.h"

void Trace::addCall(const TraceCall& call)
{
    calls.push_back(call);
}

void Trace::sortTrace()
{
    std::sort(calls.begin(), calls.end(), [](const TraceCall& a, const TraceCall& b)
    {
        if (a.calls != b.calls) return a.calls > b.calls;
        if (a.module != b.module) return a.module < b.module;
        return a.method < b.method;
    });
}

void Trace::printTrace() const
{
    int moduleWidth = std::max(longestMethodName(), static_cast<int>(std::string("Module").size()));
    int methodWidth = std::max(longestMethodName(), static_cast<int>(std::string("Method").size()));
    int callsWidth = std::to_string(maximumCalls()).size();
    int sharedWidth = 10;

    uint64_t total = totalCalls();

    std::ostringstream headerLine;
    headerLine << std::setw(moduleWidth) << std::left << "Module" << " | "
               << std::setw(methodWidth) << std::left << "Method" << " | "
               << std::setw(callsWidth) << std::right << "Calls" << " | "
               << std::setw(sharedWidth) << std::right << "Shared" << " | "
               << std::setw(sharedWidth) << std::right << "Total" << " | "
               << std::setw(sharedWidth) << std::right << "Percentage";

    std::ostringstream separatorLine;
    separatorLine << std::string(moduleWidth, '-') << "-+-"
                  << std::string(methodWidth, '-') << "-+-"
                  << std::string(callsWidth, '-') << "-+-"
                  << std::string(sharedWidth, '-') << "-+-"
                  << std::string(sharedWidth, '-') << "-+-"
                  << std::string(sharedWidth, '-');

    std::cout << headerLine.str() << "\n" << separatorLine.str() << "\n";

    for (const auto& call : calls)
    {
        float share = total > 0 ? (100.0f * call.calls) / total : 0.0f;
        std::cout << std::setw(moduleWidth) << std::left << call.module << " | "
                  << std::setw(methodWidth) << std::left << call.method << " | "
                  << std::setw(callsWidth) << std::right << call.calls << " | "
                  << std::setw(sharedWidth) << std::right << std::fixed << std::setprecision(2) << share << " | "
                  << std::setw(sharedWidth) << std::right << total << " | "
                  << std::setw(sharedWidth) << std::right << std::fixed << std::setprecision(2) << (100.0f * total) / total << "\n";
    }
}

int Trace::longestModuleName() const
{
    return std::max_element(calls.begin(), calls.end(), [](const TraceCall& a, const TraceCall& b)
    {
        return a.module.size() < b.module.size();
    })->module.size();
}

int Trace::longestMethodName() const
{
    return std::max_element(calls.begin(), calls.end(), [](const TraceCall& a, const TraceCall& b)
    {
        return a.method.size() < b.method.size();
    })->method.size();
}

uint64_t Trace::totalCalls() const
{
    return std::accumulate(calls.begin(), calls.end(), 0, [](uint64_t total, const TraceCall& call)
    {
        return total + call.calls;
    });
}

uint64_t Trace::maximumCalls() const
{
    return std::max_element(calls.begin(), calls.end(), [](const TraceCall& a, const TraceCall& b)
    {
        return a.calls < b.calls;
    })->calls;
}

bool Trace::empty()
{
    return calls.empty();
}

