#ifndef KERNELTRACER_DTRACERUNNER_H
#define KERNELTRACER_DTRACERUNNER_H

#include <cstdio>
#include <array>
#include <stdexcept>
#include <string>
#include <set>

/// @brief Class to run DTrace scripts and retrieve the output.
class DTraceRunner
{
public:
    DTraceRunner(std::string script, std::string  chrootPath);

    /**
     * @brief Run the DTrace script and return the output.
     * @return -> The output of the DTrace script.
     */
    [[nodiscard]] std::string runTrace() const;

    /**
     * @brief Get the available DTrace probes.
     * @return -> The available DTrace probes.
     */
    static std::string getAvailableProbes();

    /**
     * @brief Create a DTrace script from the available probes.
     * @param availableProbes -> The available DTrace probes.
     * @return -> The DTrace script.
     */
    static std::string createDTraceScriptFromProbes(const std::set<std::string>& availableProbes);

    /**
     * @brief Check the SIP status.
     * @return -> True if SIP is enabled, false otherwise.
     */
    static bool checkSIPStatus();

    /**
     * @brief Prepare the chroot environment.
     * @return -> The path to the chroot environment.
     */
    static std::string prepareChrootEnvironment();

private:
    std::string dTraceScript;
    std::string chrootPath;

    /**
     * @brief Execute a command and return the output.
     * @param command -> The command to execute
     * @return -> The output of the command.
     */
    static std::string executeCommand(const std::string& command);
};

#endif //KERNELTRACER_DTRACERUNNER_H
