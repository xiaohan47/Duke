#include <cstdio>
#include <cstdlib>
#include <deque>
#include <istream>
#include <queue>
#include <stack>
#include <vector>

#include "page.hpp"

using namespace std;

class story {
 private:
  vector<page> pages;
  size_t num_pages;

  //The parameters below are using to
  //set up the group structure of the story
  vector<int> visited;
  class structure {
   public:
    vector<vector<int> > s;
    int num_lev;
    structure() : num_lev(0){};
  };
  vector<structure> group;
  size_t num_groups;

  //paths using to store the valid no-circle
  //path to win.
  vector<vector<int> > paths;

 public:
  story() : num_pages(0), num_groups(0){};
  void read_story(char * filename);
  void story_group();
  structure story_structure(int n);
  bool find_1D(int f, vector<int> q);
  bool find_2D(int f, vector<vector<int> > s);
  void story_valid();
  void win_lose();
  void play_story();
  int find_depth(int pid);
  void depth_pages();
  void story_paths();
  int find_choice(int from, int to);
  void print_paths();
  ~story(){};
};
