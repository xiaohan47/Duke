#include "story.hpp"

#include <algorithm>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <istream>
#include <queue>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

using namespace std;

//Function read_story: read the story from valid
//dirname, and parsed into the pages, fullfill
//the story class

void story::read_story(char * dirname) {
  //check if page1 exist
  string p1 = +dirname;
  p1.append("/page1.txt");
  char * p1_name = &p1[0];
  ifstream page1(p1_name);
  if (!page1) {
    cerr << "Cannot open the directory.\n";
    exit(EXIT_FAILURE);
  }

  //check all valid file in the directory
  string dir_name = +dirname;
  dir_name.append("/page");
  int i = 1;
  while (true) {
    ostringstream stream;
    stream << i;
    string is = stream.str();
    string file_name = dir_name + is + ".txt";
    char * file_name_char = &file_name[0];
    ifstream file(file_name_char);

    //if the next page is not in the
    //directory, finish reading the
    //pages.
    if (!file) {
      break;
    }
    else {
      page new_page;
      num_pages++;
      char * file_name_char = &file_name[0];
      new_page.parse_page(file_name_char);
      pages.push_back(new_page);
    }
    i++;

    //limits on the number of page, cannot
    //over the max int in the current
    //platform
    if (i == INT_MAX) {
      cerr << "The num of pages exceed the INT_MAX in this computer.\n";
      exit(EXIT_FAILURE);
    }
  }

  //several functions to call to check the
  //validity of the story;set up the group
  //structure of the story and find the
  //no-cycle paths of the story
  win_lose();
  story_group();
  story_valid();
  story_paths();
}

//Function win_lose: check if the current
//story have at least one "WIN" and one "LOSE"

void story::win_lose() {
  int w = 0;
  int l = 0;
  for (size_t i = 0; i < num_pages; i++) {
    if (pages[i].win == true) {
      w++;
    }
    if (pages[i].lose == true) {
      l++;
    }
  }
  if (w == 0 || l == 0) {
    cerr << "A valid story should has at least one WIN and one LOSE.\n";
    exit(EXIT_FAILURE);
  }
}

//Function find_1D: a helper function will be used
//in multiple plase, to check if certain page is
//in the 1D vector of not.

bool story::find_1D(int f, vector<int> v) {
  for (vector<int>::iterator it = v.begin(); it != v.end(); ++it) {
    if ((*it) == f) {
      return 1;
    }
  }
  return 0;
}

//Function find_2D: a helper function will be used
//in multiple plase, to check if certain page is
//in the 2D vector of not.

bool story::find_2D(int f, vector<vector<int> > s) {
  for (vector<vector<int> >::iterator it1 = s.begin(); it1 != s.end(); ++it1) {
    for (vector<int>::iterator it2 = (*it1).begin(); it2 != (*it1).end(); ++it2) {
      if ((*it2) == f) {
        return 1;
      }
    }
  }
  return 0;
}

//Function story_group: to create the highest level
//of the sturcture in story: group. Each group will
//contains a valid structure. For example, if there
//are a group which not reached by page1, but refer
//to each other.

void story::story_group() {
  while (visited.size() != num_pages) {
    for (size_t i = 1; i <= num_pages; i++) {
      if (find_1D(i, visited) == 0) {
        group.push_back(story_structure(i));
        num_groups++;
      }
    }
  }
}

//Function story_structure: returns a structure class,
//which contains a 2D vector;Each of the vector contains
//a level of the current group of story, and each level
//contains the page num in current level.

story::structure story::story_structure(int n) {
  //adding first unvisited page num into a queue following the choice sequence
  //using queue to help generate the structure
  queue<int> q;
  q.push(n);
  visited.push_back(n);
  structure cur_s;

  //if queue if not empty, will iterate each page and there choices
  while (!q.empty()) {
    //current level size will equal to the q size before adding
    //the next leve's choice into it.
    int cur_level = q.size();
    cur_s.s.push_back(vector<int>());
    cur_s.num_lev++;
    for (int i = 0; i < cur_level; i++) {
      int p = q.front();
      //pop the checked page out of the queue
      q.pop();
      cur_s.s.back().push_back(p);
      //iterate from the choice
      if (pages[p - 1].num_choice != 0) {
        for (size_t j = 0; j < pages[p - 1].num_choice; j++) {
          if (find_1D(pages[p - 1].choice[j].next_pnum, visited) == 0) {
            if (pages[p - 1].choice[j].next_pnum > num_pages) {
              cerr << "A choice point to an unkown page.\n";
              exit(EXIT_FAILURE);
            }
            q.push(pages[p - 1].choice[j].next_pnum);
            visited.push_back(pages[p - 1].choice[j].next_pnum);
          }
        }
      }
    }
  }
  return cur_s;
}

//Function story_valid: checking if the given story is valid.

void story::story_valid() {
  int find = 0;
  for (size_t i = 1; i <= num_pages; i++) {
    //search in each group, if find the element,reset flag find
    //then go to find next page.
    for (size_t j = 0; j < num_groups; j++) {
      find += find_2D(i, group[j].s);
    }
    if (find == 0) {
      cerr << "There are certain pages do not have parent.\n";
      exit(EXIT_FAILURE);
    }
    find = 0;
  }
}

//Function play_story: ask user to play the story as request.

void story::play_story() {
  //always start with page 1
  pages[0].print_page();
  size_t i = 0;

  while (true) {
    size_t user;
    cin >> user;
    if (user > pages[i].num_choice || user < 1) {
      cerr << "Please enter the valid choice number.\n";
      exit(EXIT_FAILURE);
    };

    size_t next_page = pages[i].choice[user - 1].next_pnum;
    pages[next_page - 1].print_page();
    if (pages[next_page - 1].win == true || pages[next_page - 1].lose == true) {
      exit(EXIT_SUCCESS);
    }
    i = next_page - 1;
  }
}

//Funtion find_depth: find the depth for each page,
//because the group already contains the structure
//class, which works as levels (2D vector). Therefore
//we only need to read the first group(because if there
//has a second group, the others will not reachable
//from page1.

int story::find_depth(int n) {
  if (find_2D(n, group[0].s) == 0) {
    return -1;
  }

  int total_depth = group[0].num_lev;
  int depth;
  for (depth = 0; depth < total_depth; depth++) {
    if (find_1D(n, group[0].s[depth]) == 1) {
      return depth;
    }
  }
  return -1;
}

//Function depth_pages: print out each pages depth by
//calling the function find_depth.

void story::depth_pages() {
  for (size_t i = 0; i < num_pages; i++) {
    int depth = find_depth(i + 1);
    if (depth != -1) {
      cout << "Page " << i + 1 << ":" << depth << endl;
    }
    if (depth == -1) {
      cout << "Page " << i + 1 << " is not reachable" << endl;
    }
  }
}

//Function story_paths: using stack of vector<int> to set up
//a DFS in the current story.

void story::story_paths() {
  stack<vector<int> > todo;
  //a local vector visited_page to track the visited page.
  //I am not out "WIN" page here, because there always need
  //to check if you are win in order to track the valid path
  vector<int> visited_page;
  vector<int> first_page;
  first_page.push_back(1);
  todo.push(first_page);

  while (!todo.empty()) {
    vector<int> cur_path = todo.top();
    todo.pop();
    int cur_page = cur_path.back();

    //if current page to read is win, the cur_path is a valid
    //path to record.
    if (pages[cur_page - 1].win == true) {
      paths.push_back(cur_path);
    }
    //if current page is for choosing, iterate each choice and
    //push the next path into the stack
    if (pages[cur_page - 1].win == false && pages[cur_page - 1].lose == false &&
        pages[cur_page - 1].num_choice != 0) {
      if (find_1D(cur_page, visited_page) == 0) {  //if the cur_page not in the visited
        visited_page.push_back(cur_page);
        for (size_t i = 0; i < pages[cur_page - 1].num_choice; i++) {
          vector<int> next_path = cur_path;
          next_path.push_back(pages[cur_page - 1].choice[i].next_pnum);
          todo.push(next_path);
        }
      }
    }
    //if current page to read is lose, just need to push one
    //time of the lose page to visited.
    if (pages[cur_page - 1].lose == true) {
      if (find_1D(cur_page, visited_page) == 0) {  //if the cur_page not in the visited
        visited_page.push_back(cur_page);
      }
    }
  }
}

//Function find_choice:a helper function for print_path;
//it will return the choice number from page "from" to
//page "to".

int story::find_choice(int from, int to) {
  for (size_t i = 0; i < pages[from - 1].num_choice; i++) {
    int next = pages[from - 1].choice[i].next_pnum;
    if (to == next) {
      return i + 1;
    }
  }
  //just a double check in case something went wrong
  //when reading the story
  cerr << "Some wrong with find_choice" << endl;
  return EXIT_FAILURE;
}

//Function print_paths:print the valid paths as request.

void story::print_paths() {
  if (paths.size() == 0) {
    cout << "This story is unwinnable!" << endl;
  }
  else {
    for (size_t i = 0; i < paths.size(); i++) {
      for (size_t j = 0; j < paths[i].size() - 1; j++) {
        int next = find_choice(paths[i][j], paths[i][j + 1]);
        cout << paths[i][j] << "(" << next << ")"
             << ",";
      }
      cout << paths[i][paths[i].size() - 1] << "(win)" << endl;
    }
  }
}
