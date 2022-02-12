#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

class page {
 private:
  string context;

 public:
  bool win;
  bool lose;
  size_t page_num;

  //a class contains all choices for certain page
  class choice_info {
   public:
    size_t next_pnum;
    string choice_context;
    choice_info() : next_pnum(0){};
    choice_info(int num) : next_pnum(num){};
    ~choice_info(){};
  };
  vector<choice_info> choice;
  size_t num_choice;

  page() : win(false), lose(false), page_num(0), num_choice(0){};
  ~page(){};

  //parse_page will parse page to vector of string of each line;
  //where set_page will read it and make it as a page;
  void parse_page(char * file_name);
  void set_page(vector<string> line_vec);
  void print_page();
};
