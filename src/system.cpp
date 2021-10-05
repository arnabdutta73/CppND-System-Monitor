#include "system.h"

#include <unistd.h>

#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

// Returns the system's CPU
Processor& System::Cpu() { return cpu_; }

// Returns a container composed of the system's processes
vector<Process>& System::Processes() {
  processes_.clear();
  vector<int> all_pids = LinuxParser::Pids();
  for (auto pid : all_pids) {
    Process process(pid);
    processes_.push_back(process);
  }
  std::sort(processes_.rbegin(), processes_.rend());
  return processes_;
}

// Returns the system's kernel identifier (string)
std::string System::Kernel() { return LinuxParser::Kernel(); }

// Returns the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// Returns the operating system name
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

// Returns the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// Returns the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// Returns the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }