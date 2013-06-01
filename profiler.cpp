#include "profiler.hpp"

#include <iomanip>
#include <limits>
#include <algorithm>

#include <sys/time.h>

namespace dm
{

bool Profiler::enabled_ = false;
std::map<std::string, std::vector<ProfilerEntry> > Profiler::entries_;
std::map<std::string, bool> Profiler::running_;

static bool aggregated_profiler_entry_total_cmp(const AggregatedProfilerEntry& a,
                                                const AggregatedProfilerEntry& b) {
  return (a.avg_ms > b.avg_ms);
};

static int64_t timestamp_now() {
  struct timeval tv;
  gettimeofday (&tv, NULL);
  return static_cast<int64_t>(tv.tv_sec * 1000000 + tv.tv_usec);
}

void Profiler::enable() { Profiler::enabled_ = true; }
void Profiler::disable() { enabled_ = false; }

int64_t Profiler::tictoc(const char * name) {
  if (!enabled_) { return 0; }
  int64_t timestamp = timestamp_now();
  std::map<std::string, bool>::iterator running_itr = running_.find(name);
  if (running_itr == running_.end() ||
      !running_itr->second) {
    // create new entry, get timer running
    entries_[name].push_back(ProfilerEntry(timestamp));
    running_[name] = true;
    return 0;
  }
  // timer running, stop it.
  ProfilerEntry& entry(entries_[name].back());
  entry.end_time = timestamp;
  running_itr->second = false;
  return entry.end_time-entry.start_time;
}

void Profiler::aggregate_entries(std::vector<AggregatedProfilerEntry>* aggregated) {
  if (!enabled_) { return; }
  for (ProfilerEntryMap::iterator itr = entries_.begin();
       itr != entries_.end();
       ++itr) {
    std::vector<ProfilerEntry>& name_entries(itr->second);
    AggregatedProfilerEntry ag;
    ag.name = itr->first;
    ag.min_ms = std::numeric_limits<double>::max();
    ag.max_ms = -1;
    ag.total_ms = 0.;
    ag.num_calls = static_cast<int>(name_entries.size());
    for (size_t i=0; i < name_entries.size(); ++i) {
      double delta_t = static_cast<double>(name_entries[i].end_time - name_entries[i].start_time)*1e-6;
      ag.total_ms += delta_t;
      ag.min_ms = std::min(ag.min_ms, delta_t);
      ag.max_ms = std::max(ag.max_ms, delta_t);
    }
    ag.avg_ms = ag.total_ms / ag.num_calls;
    aggregated->push_back(ag);
  }
}

void Profiler::print_aggregated(std::ostream& os) {
  if (!enabled_) { return; }
  std::vector<AggregatedProfilerEntry> aggregated;
  aggregate_entries(&aggregated);
  std::sort(aggregated.begin(), aggregated.end(), aggregated_profiler_entry_total_cmp);
  os << "\n\n";
  os << std::setw(60) << std::setfill(' ') << "Description";
  os << std::setw(20) << std::setfill(' ') << "Calls";
  os << std::setw(20) << std::setfill(' ') << "Total s";
  os << std::setw(20) << std::setfill(' ') << "Avg s";
  os << std::setw(20) << std::setfill(' ') << "Min s";
  os << std::setw(20) << std::setfill(' ') << "Max s";
  os << "\n";
  for (size_t i=0; i < aggregated.size(); ++i) {
    AggregatedProfilerEntry& ag(aggregated[i]);
    os << std::setw(60) << std::setfill(' ') << ag.name;
    os << std::setw(20) << std::setprecision(5) << std::setfill(' ') << ag.num_calls;
    os << std::setw(20) << std::setprecision(5) << std::setfill(' ') << ag.total_ms;
    os << std::setw(20) << std::setprecision(5) << std::setfill(' ') << ag.avg_ms;
    os << std::setw(20) << std::setprecision(5) << std::setfill(' ') << ag.min_ms;
    os << std::setw(20) << std::setprecision(5) << std::setfill(' ') << ag.max_ms;
    os << "\n";
  }
  os << "\n";
}

void Profiler::print_all(std::ostream& os) {
  if (!enabled_) { return; }
  os << "start_time; ";
  os << "description; ";
  os << "duration";
  os << "\n";
  for (ProfilerEntryMap::iterator itr = entries_.begin();
       itr != entries_.end();
       ++itr) {
    std::string name(itr->first);
    std::vector<ProfilerEntry>& name_entries(itr->second);
    for (size_t i=0; i < name_entries.size(); ++i) {
      os << name_entries[i].start_time << "; ";
      os << name << "; ";
      os << (name_entries[i].end_time - name_entries[i].start_time);
      os << "\n";
    }
  }
}

}
