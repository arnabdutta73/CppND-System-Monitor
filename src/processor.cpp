#include "processor.h"

#include "linux_parser.h"

// Returns the aggregate CPU utilization
float Processor::Utilization() {
  Idle = LinuxParser::IdleJiffies();
  NonIdle = LinuxParser::Jiffies();
  Total = Idle + NonIdle;

  PrevTotal = PrevIdle + PrevNonIdle;

  // differentiate: actual value minus the previous one
  totald = Total - PrevTotal;
  idled = Idle - PrevIdle;

  PrevIdle = Idle;
  PrevNonIdle = NonIdle;

  return float(totald - idled) / float(totald);
}