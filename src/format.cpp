#include "format.h"

#include <time.h>
#include <unistd.h>
#include <iomanip>

#include <string>

using std::string;
using std::to_string;

// Convert Time from Seconds to HH:MM:SS
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
  long minutes;
  long hours;
  hours = seconds/3600;
  seconds = seconds%3600;
  minutes = seconds/60;
  seconds = seconds%60;
// taken from https://stackoverflow.com/questions/48310746/converting-time-into-hours-minutes-and-seconds
  std::ostringstream stream;
  stream << std::setw(2) << std::setfill('0') << hours << ":" 
     << std::setw(2) << std::setfill('0') << minutes << ":"
     << std::setw(2) << std::setfill('0') << seconds;
  return stream.str();
}