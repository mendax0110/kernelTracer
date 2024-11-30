#include "../include/Trace.h"
#include "../include/DTraceRunner.h"
#include <iostream>
#include <sstream>
#include <string>
#include <regex>
#include <set>

int main(int argc, char* argv[])
{
    const std::regex callRegex(R"(^([\w\d`]+)`([\w\d]+)\s+(\d+)$)");
    Trace trace;

    bool sipEnabled = DTraceRunner::checkSIPStatus();
    std::string baseSystem;

    if (sipEnabled)
    {
        try
        {
            baseSystem = DTraceRunner::prepareChrootEnvironment();
        }
        catch (const std::exception& ex)
        {
            std::cerr << "Chroot environment setup failed: " << ex.what() << std::endl;
            return EXIT_FAILURE;
        }
    }

    std::cerr << "Using DTrace within chroot: " << baseSystem << std::endl;

    DTraceRunner runner("", baseSystem);
    std::string availableProbesOutput = DTraceRunner::getAvailableProbes();

    if (availableProbesOutput.empty())
    {
        std::cerr << "Failed to retrieve available probes. Ensure DTrace is installed and accessible." << std::endl;
        return EXIT_FAILURE;
    }

    std::set<std::string> availableProbes;
    std::istringstream probeStream(availableProbesOutput);
    std::string probeLine;

    while (std::getline(probeStream, probeLine))
    {
        std::string probeName = probeLine.substr(0, probeLine.find(' '));
        availableProbes.insert(probeName);
    }

    std::string dtraceScript = DTraceRunner::createDTraceScriptFromProbes(availableProbes);

    if (dtraceScript.empty())
    {
        std::cerr << "No compatible probes found. Exiting." << std::endl;
        return EXIT_FAILURE;
    }

    try
    {
        runner = DTraceRunner(dtraceScript, baseSystem);
        std::string output = runner.runTrace();
        if (output.empty())
        {
            std::cerr << "DTrace produced no output. Ensure the script uses compatible probes." << std::endl;
            return EXIT_FAILURE;
        }

        std::istringstream stream(output);
        std::string line;
        while (std::getline(stream, line))
        {
            std::cerr << "Parsing line: " << line << std::endl;
            std::smatch match;
            if (std::regex_match(line, match, callRegex))
            {
                trace.addCall(TraceCall(match[1], match[2], std::stoull(match[3])));
                std::cerr << "Matched: " << match[1] << ", " << match[2] << ", " << match[3] << std::endl;
            }
        }

        if (trace.empty())
        {
            std::cerr << "No valid data parsed from trace." << std::endl;
            return EXIT_FAILURE;
        }

        trace.sortTrace();
        trace.printTrace();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}