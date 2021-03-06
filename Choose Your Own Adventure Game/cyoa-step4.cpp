#include <cstdlib>
#include <iostream>

#include "story.hpp"

using namespace std;

int main(int argc, char ** argv) {
  if (argc != 2) {
    cerr << "Please only input one directory name to read.\n";
    return EXIT_FAILURE;
  }
  story s;
  s.read_story(argv[1]);
  s.print_paths();
  return EXIT_SUCCESS;
}
