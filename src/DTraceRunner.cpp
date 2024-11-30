#include "../include/DTraceRunner.h"
#include <iostream>
#include <regex>
#include <algorithm>
#include <cstdlib>
#include <utility>

DTraceRunner::DTraceRunner(std::string script, std::string  chrootPath)
        : dTraceScript(std::move(script)), chrootPath(std::move(chrootPath))
{

}

std::string DTraceRunner::runTrace() const
{
    std::string command = "/usr/sbin/dtrace -n '" + dTraceScript + "'";
    if (!chrootPath.empty())
    {
        command = "sudo chroot " + chrootPath + " " + command;
    }
    return executeCommand(command);
}

std::string DTraceRunner::executeCommand(const std::string& command)
{
    std::vector<char> buffer(128);
    std::string result;
    std::shared_ptr<FILE> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe)
    {
        throw std::runtime_error("popen() failed!");
    }
    while (!feof(pipe.get()))
    {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
        {
            result += buffer.data();
        }
    }
    return result;
}

bool DTraceRunner::checkSIPStatus()
{
    std::string sipStatus = executeCommand("csrutil status");
    std::cerr << "SIP Status: " << sipStatus << std::endl;
    return sipStatus.find("enabled") != std::string::npos;
}

std::string DTraceRunner::prepareChrootEnvironment()
{
    std::string baseSystem = "/tmp/macos_base";
    std::string createCmd =
            "hdiutil attach -owners on /Applications/Install\\ macOS.app/Contents/SharedSupport/BaseSystem.dmg "
            "-mountpoint " + baseSystem + " -shadow /tmp/BaseSystem.shadow";

    if (std::system(createCmd.c_str()) != 0)
    {
        throw std::runtime_error("Failed to create chroot environment.");
    }

    std::string removeSignaturesCmd = "sudo find " + baseSystem + " -exec codesign --remove-signature {} \\; > /dev/null 2>&1";
    if (std::system(removeSignaturesCmd.c_str()) != 0)
    {
        throw std::runtime_error("Failed to remove code signatures in chroot.");
    }

    return baseSystem;
}

std::string DTraceRunner::getAvailableProbes()
{
    std::string command = "/usr/sbin/dtrace -l";
    std::string result = executeCommand(command);

    if (result.empty())
    {
        std::cerr << "Error: Failed to retrieve probes. Ensure that dtrace is installed and accessible." << std::endl;
        return "";
    }

    std::cerr << "DTrace probes output: " << std::endl;
    std::cerr << result << std::endl;

    return result;
}

std::string DTraceRunner::createDTraceScriptFromProbes(const std::set<std::string>& availableProbes)
{
    std::string dtraceScript;

    if (availableProbes.count("profile:::tick-1000hz"))
    {
        dtraceScript = R"(
            #pragma D option quiet
            profile:::tick-1000hz
            {
                @ticks = count();
            }
            END
            {
                printa("Ticks: %@d\n", @ticks);
            })";
    }
    else if (availableProbes.count("syscall:::entry"))
    {
        dtraceScript = R"(
            #pragma D option quiet
            syscall:::entry
            {
                @syscalls[probefunc] = count();
            }
            END
            {
                printa("%s %@d\n", @syscalls);
            })";
    }
    else
    {
        std::cerr << "No compatible probes found. Ensure SIP is disabled or run on a system with unrestricted DTrace." << std::endl;
    }

    return dtraceScript;
}
