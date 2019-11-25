#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include<iostream>
#include "linux_parser.h"

using std::stof;
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
  string os, version, kernel;
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
        pids.emplace_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
    string line1, line2;
    string key, units;
    string MemTotal, MemFree;
    std::ifstream filestream(kProcDirectory + kMeminfoFilename);
    if (filestream.is_open()) {
        std::getline(filestream, line1);
        //std::cout << "line1: " << line1 << "\n";
        std::istringstream linestream1(line1);
        linestream1 >> key >> MemTotal >> units;
        //std::cout << "MemTotal: " << MemTotal << "\n";

        std::getline(filestream, line2);
        //std::cout << "line2: " << line2 << "\n";
        std::istringstream linestream2(line2);
        linestream2 >> key >> MemFree >> units;
        //std::cout << "MemFree: " << MemFree << "\n";

        filestream.close();

    }
    return (stof(MemTotal) - stof(MemFree)) / stof(MemTotal);
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
    string uptime, idletime;
    string line;
    std::string::size_type sz;   // alias of size_t

    std::ifstream stream(kProcDirectory + kUptimeFilename);
    if (stream.is_open()) {
      std::getline(stream, line);
      std::istringstream linestream(line);
      //std::cout << "line: " << line << "\n";
      linestream >> uptime >> idletime;
    }
    //std::cout << "uptime: " << std::stol (uptime,&sz) << "\n";
    return std::stol (uptime,&sz);
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
    long system_jiffies = ActiveJiffies() + IdleJiffies();
    return system_jiffies;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
    std::vector<string> cpu_vector = LinuxParser::CpuUtilization();

    long acitve_jiffies = std::stol(cpu_vector[CPUStates::kUser_]) + std::stol(cpu_vector[CPUStates::kNice_])
            + std::stol(cpu_vector[CPUStates::kSystem_]) + std::stol(cpu_vector[CPUStates::kIRQ_]) + std::stol(cpu_vector[CPUStates::kSoftIRQ_])
            + std::stol(cpu_vector[CPUStates::kSteal_]) + std::stol(cpu_vector[CPUStates::kGuest_]) + std::stol(cpu_vector[CPUStates::kGuestNice_]);

    return acitve_jiffies;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
    std::vector<string> cpu_vector = LinuxParser::CpuUtilization();
    long idle_jiffies = std::stol(cpu_vector[CPUStates::kIdle_]) + std::stol(cpu_vector[CPUStates::kIOwait_]);
    return idle_jiffies;
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
    std::vector<string> cpu_vector;
    string line;
    string value;
    std::ifstream filestream(kProcDirectory + kStatFilename);

    if (filestream.is_open()) {
        while (std::getline(filestream, line)) {
            std::istringstream linestream(line);
            while (linestream >> value) {
                if(value == "cpu") {
                    while(linestream >> value)
                        cpu_vector.push_back(value);
                    return cpu_vector;
                }
            }
        }
    }
    return cpu_vector;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
    string line;
    string key;
    string value;
    std::ifstream filestream(kProcDirectory + kStatFilename);
    if (filestream.is_open()) {
      while (std::getline(filestream, line)) {
        std::istringstream linestream(line);
        while (linestream >> key >> value) {
          //std::cout << key << "\n";
          if (key == "processes") {
            return std::stoi(value);
          }
        }
      }
    }
    return std::stoi(value);
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
    string line;
    string key;
    string value;
    std::ifstream filestream(kProcDirectory + kStatFilename);
    if (filestream.is_open()) {
      while (std::getline(filestream, line)) {
        std::istringstream linestream(line);
        while (linestream >> key >> value) {
          //std::cout << key << "\n";
          if (key == "procs_running") {
            return std::stoi(value);
          }
        }
      }
    }
    return std::stoi(value);
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
// pid[[maybe_unused]]
string LinuxParser::Command(int pid) {
    string line;
    std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
    if (filestream.is_open()) {
        std::getline(filestream, line);
        filestream.close();
    }
    return line;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
    string line;
    string key;
    string value, units;
    std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
    if (filestream.is_open()) {
      while (std::getline(filestream, line)) {
        std::istringstream linestream(line);
        while (linestream >> key >> value >> units) {
          if (key == "VmSize:") {
             // std::cout << "value: " << value <<"\n";
            return value;
          }
        }
      }
    }


    return string();
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
    string line;
    string key;
    string value;
    std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
    if (filestream.is_open()) {
      while (std::getline(filestream, line)) {
        std::istringstream linestream(line);
        while (linestream >> key >> value) {
          if (key == "Uid:") {
              //std::cout << "value: " << value <<"\n";
            return value;
          }
        }
      }
    }
    return string();
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
    string line;
    string token = ":x:" + to_string(pid) + ":";
    //std::cout << "token: " << token << "\n";
    std::ifstream filestream(kPasswordPath);

    if (filestream.is_open()) {
      while (std::getline(filestream, line)) {
        std::istringstream linestream(line);
        std::size_t found = line.find(token);

        //std::cout << "found: " << found << "\n";
        if (found!=std::string::npos) {
            //std::cout << "line: " << line << "\n";
            std::string user = line.substr (0,found);
            //std::cout << "found: " << found << ", user: " << user << '\n';
            return user;
        }


      }
    }
    return string();
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
    //http://man7.org/linux/man-pages/man5/proc.5.html

    long int uptime{0};
    string value;
    std::ifstream filestream(LinuxParser::kProcDirectory + to_string(pid) + LinuxParser::kStatFilename);

    if (filestream.is_open()) {
      for (int i = 0; filestream >> value; ++i)
        if (i == 13) {
          uptime = stol(value) / sysconf(_SC_CLK_TCK);
          //std::cout << "value: " << value  << "\n";
          return uptime;
        }
    }

    return 0;
}
