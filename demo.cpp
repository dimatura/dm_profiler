#include <iostream>

#include "profiler.hpp"

static long rec_fibonacci(long i) {
  if (i==1) return 1;
  if (i==2) return 1;
  return rec_fibonacci(i-1) + rec_fibonacci(i-2);
}

static long linear_fibonacci(long i) {
  long aux[i];
  aux[0] = aux[1] = 1;
  for (int j=2; j < i; ++j) {
    aux[j] = aux[j-2] + aux[j-1];
  }
  return aux[i-1];
}

int main(int argc, const char *argv[]) {

  dm::Profiler::enable();

  long rfsum = 0, lfsum = 0;

  for (int i=1; i < 40; ++i) {
    dm::Profiler::tictoc("rec_fibonacci");
    rfsum += rec_fibonacci(i);
    dm::Profiler::tictoc("rec_fibonacci");
  }

  for (int i=1; i < 40; ++i) {
    dm::Profiler::tictoc("linear_fibonacci");
    lfsum += linear_fibonacci(i);
    dm::Profiler::tictoc("linear_fibonacci");
  }

  std::cerr << "rfsum = " << rfsum << "\n";
  std::cerr << "lfsum = " << lfsum << "\n";

  dm::Profiler::print_aggregated(std::cout);

  return 0;
}
