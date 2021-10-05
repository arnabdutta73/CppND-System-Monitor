#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) { this->pid = pid; }

// Returns this process's ID.
int Process::Pid() { return pid; }

// Returns this process's CPU utilization.
float Process::CpuUtilization() const {
  return LinuxParser::CpuUtilization(pid);
}

// Returns the command that generated this process.
string Process::Command() { return LinuxParser::Command(pid); }

// Returns this process's memory utilization.
string Process::Ram() { return LinuxParser::Ram(pid); }

// Returns the user (name) that generated this process.
string Process::User() { return LinuxParser::User(pid); }

// Returns the age of this process (in seconds).
long int Process::UpTime() { return LinuxParser::UpTime(pid); }

// Overloads the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
  return (CpuUtilization() < a.CpuUtilization());
}