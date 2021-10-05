#include "linux_parser.h"

#include <dirent.h>
#include <time.h>
#include <unistd.h>

#include <sstream>
#include <string>
#include <vector>

using std::stof;
using std::stoi;
using std::stol;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os;
  string kernel;
  string version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Reads and returns the system memory utilization.
float LinuxParser::MemoryUtilization() {
  string line;
  string key;
  string value;
  float total_memory = 0.0;
  float free_memory = 0.0;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "MemTotal:") {
        total_memory = std::stof(value);
      }
      if (key == "MemFree:") {
        free_memory = std::stof(value);
      }
    }
    return (total_memory - free_memory) / total_memory;
  }
  return 0.0;
}

// Reads and returns the system uptime.
long LinuxParser::UpTime() {
  string line;
  string up_time;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> up_time;
    return std::stol(up_time);
  }
  return 0;
}

// Reads and returns the number of jiffies for the system.
long LinuxParser::Jiffies() {
  long total = 0;
  vector<string> cpu_utilization = LinuxParser::CpuUtilization();
  total = stol(cpu_utilization[kUser_]) + stol(cpu_utilization[kNice_]) +
          stol(cpu_utilization[kSystem_]) + stol(cpu_utilization[kIRQ_]) +
          stol(cpu_utilization[kSoftIRQ_]) + stol(cpu_utilization[kSteal_]);
  return total;
}

// Reads and returns the number of active jiffies for a PID.
long LinuxParser::ActiveJiffies(int pid) {
  string line;
  string value;
  vector<string> states;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      if (value != "cpu") {
        states.push_back(value);
      }
    }
    long total = stol(states[kUser_]) + stol(states[kNice_]) +
                 stol(states[kSystem_]) + stol(states[kIRQ_]) +
                 stol(states[kSoftIRQ_]) + stol(states[kSteal_]) -
                 stol(states[kIdle_]) - stol(states[kIOwait_]);
    return total;
  }
  return 0;
}

// Reads and returns the number of active jiffies for the system.
long LinuxParser::ActiveJiffies() {
  return LinuxParser::Jiffies() - LinuxParser::IdleJiffies();
}

// Reads and returns the number of idle jiffies for the system.
long LinuxParser::IdleJiffies() {
  long total = 0;
  vector<string> cpu_utilization = LinuxParser::CpuUtilization();
  total = stol(cpu_utilization[kIdle_]) + stol(cpu_utilization[kIOwait_]);
  return total;
}

// Reads and returns the CPU utilisation for each PID.
float LinuxParser::CpuUtilization(int pid) {
  string line;
  string value;
  vector<string> states;
  float utime;
  float stime;
  float cutime;
  float cstime;
  float starttime;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for (int i = 0; linestream >> value; i++) {
      switch (i) {
        case 13: {
          utime = stof(value);
          break;
        }
        case 14: {
          stime = stof(value);
          break;
        }
        case 15: {
          cutime = stof(value);
          break;
        }
        case 16: {
          cstime = stof(value);
          break;
        }
        case 21: {
          starttime = stof(value);
          break;
        }
      }
    }
    float total_time = utime + stime + cutime + cstime;
    return ((total_time / sysconf(_SC_CLK_TCK)) /
                  (UpTime() - (starttime / (float)sysconf(_SC_CLK_TCK))));
  }
  return 0.0;
}

// Reads and returns CPU utilization.
vector<string> LinuxParser::CpuUtilization() {
  string line;
  string value;
  vector<string> states;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      if (value != "cpu") {
        states.push_back(value);
      }
    }
  }
  return states;
}

// Reads and returns the total number of processes.
int LinuxParser::TotalProcesses() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "processes") {
        return std::stoi(value);
      }
    }
  }
  return 0;
}

// Reads and returns the number of running processes.
int LinuxParser::RunningProcesses() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "procs_running") {
        return std::stoi(value);
      }
    }
  }
  return 0;
}

// Reads and returns the command associated with a process.
string LinuxParser::Command(int pid) {
  string line;
  string value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    // std::istringstream linestream(line);
    // linestream >> value;
  }
  return line;
}

// Reads and returns the memory used by a process.
string LinuxParser::Ram(int pid) {
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmRSS:") {
          return std::to_string(stof(value) / 1000);
        }
      }
    }
  }
  return value;
}

// Reads and returns the user ID associated with a process.
string LinuxParser::Uid(int pid) {
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid:") {
          return value;
        }
      }
    }
  }
  return string();
}

// Reads and returns the user associated with a process.
string LinuxParser::User(int pid) {
  string line;
  string user;
  string pass;
  string uid;
  string temp_uid;
  temp_uid = LinuxParser::Uid(pid);
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> user >> pass >> uid) {
        if (uid == temp_uid) {
          return user;
        }
      }
    }
  }
  return string();
}

// Reads and returns the uptime of a process.
long LinuxParser::UpTime(int pid) {
  string line;
  string value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for (int i = 0; linestream >> value; i++) {
      if (i == 21) {
        return UpTime() -  std::stol(value) / sysconf(_SC_CLK_TCK);
      }
    }
  }
  return 0;
}
