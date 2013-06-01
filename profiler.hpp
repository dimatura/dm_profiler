#ifndef _DM_PROFILER_HPP_
#define _DM_PROFILER_HPP_

#include <stdint.h>
#include <time.h>
#include <vector>
#include <map>
#include <string>
#include <iostream>

namespace dm
{

struct ProfilerEntry {
  int64_t start_time;
  int64_t end_time;
  ProfilerEntry(int64_t _start_time) :
      start_time(_start_time), end_time(-1) { }
};

struct AggregatedProfilerEntry {
  double total_ms;
  double min_ms;
  double max_ms;
  double avg_ms;
  int num_calls;
  std::string name;
};

class Profiler {
 public:
  typedef std::map<std::string, std::vector<ProfilerEntry> > ProfilerEntryMap;

  static int64_t tictoc(const char * name);

  static void enable();
  static void disable();

  static void aggregate_entries(std::vector<AggregatedProfilerEntry>* aggregated);

  static void print_aggregated(std::ostream& os);
  static void print_all(std::ostream& os);
  static void print_full(std::ostream& os);

private:
  Profiler(const Profiler& other);
  Profiler& operator=(const Profiler& other);

  static bool enabled_;

  static ProfilerEntryMap entries_;
  static std::map<std::string, bool> running_;
};

}
#endif
