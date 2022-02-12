#include <cstdlib>
#include <iostream>

#include "page.hpp"

using namespace std;

int main(int argc, char ** argv) {
  if (argc != 2) {
    cerr << "Please only input one page to read.\n";
    return EXIT_FAILURE;
  }
  page p;
  p.parse_page(argv[1]);
  p.print_page();
  return EXIT_SUCCESS;
}
