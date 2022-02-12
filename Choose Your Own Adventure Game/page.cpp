#include "page.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

//Function parse_page:  will read the input file, parsed
//it into each line of string, put into a vector, call
//set_page to setup a page
void page::parse_page(char * file_name) {
  ifstream file(file_name);
  if (!file) {
    cerr << "The file cannot open.\n";
    exit(EXIT_FAILURE);
  }

  //check if the txt file name not start with pageX.
  string file_name_str;
  file_name_str += file_name;
  int name_bef = file_name_str.rfind('/');
  string file_name_str4 = file_name_str.substr(name_bef + 1, 4);
  if (file_name_str4.compare("page") != 0) {
    cerr << "The file name should start with pageX.\n";
    exit(EXIT_FAILURE);
  }

  //store the txt file into vector<string> by line.
  string line;
  vector<string> line_vec;
  while (getline(file, line)) {
    line_vec.push_back(line);
  }
  set_page(line_vec);
}

//Function set_page:using the information to set up a page.
void page::set_page(vector<string> line_vec) {
  //It should at least have two line
  int len = line_vec.size();
  if (len <= 1) {
    cerr << "The page not format correctly: short of lines.\n";
    exit(EXIT_FAILURE);
  }

  // if the first line is "WIN"
  if (line_vec[0] == "WIN") {
    //check if the second line is the seperator "#"
    if (line_vec[1][0] == '#') {
      win = true;
      if (len != 2) {
        for (int i = 2; i < len - 1; i++) {
          context.append(line_vec[i]);
          context.append("\n");
        }
        context.append(line_vec[len - 1]);
      }
    }
    else {
      cerr << "The page not format correctly: no # after WIN.\n";
      exit(EXIT_FAILURE);
    }
  }

  //if the first line is "LOSE"
  else if (line_vec[0] == "LOSE") {
    //check if the second line is the seperator "#"
    if (line_vec[1][0] == '#') {
      lose = true;
      if (len != 2) {
        for (int i = 2; i < len - 1; i++) {
          context.append(line_vec[i]);
          context.append("\n");
        }
        context.append(line_vec[len - 1]);
      }
    }
    else {
      cerr << "The page not format correctly: no # after LOSE.\n";
      exit(EXIT_FAILURE);
    }
  }

  //if the first line is choice, the first letter of choice must
  //be valid int from 1 to 9;
  else if (line_vec[0][0] >= '1' && line_vec[0][0] <= '9') {
    int i = 0;
    num_choice = 0;

    //adding all choice to the choice class and push into choice
    //store vector
    while (line_vec[i][0] != '#') {
      size_t col_index = line_vec[i].find(':');

      // if there is no ':'
      if (col_index == line_vec[i].npos) {
        cerr << "The page not format correctly: no : find.\n";
        exit(EXIT_FAILURE);
      }

      string num_str;
      for (size_t j = 0; j < col_index; j++) {
        if (line_vec[i][j] > '9' && line_vec[i][j] < '0') {
          cerr << "The page not format correctly: the choice is not a valid integer.\n";
          exit(EXIT_FAILURE);
        }
        num_str.append(&line_vec[i][j]);
      }

      int num = atoi(num_str.c_str());

      //create new choice_info type.
      choice_info new_choice(num);
      size_t k = col_index + 1;
      size_t leng_line = line_vec[i].size();
      new_choice.choice_context = line_vec[i].substr(k, leng_line - k);
      choice.push_back(new_choice);
      num_choice++;
      i++;
    }
    //after adding the choice, adding the other parts
    //of the content
    if (len != 2) {
      for (int m = i + 1; m < len - 1; m++) {
        context.append(line_vec[m]);
        context.append("\n");
      }
      context.append(line_vec[len - 1]);
    }
  }
  else {
    cerr << "The input formate not following the request.\n";
    exit(EXIT_FAILURE);
  }
}

//Function print_page: using to print page infor as required.
void page::print_page() {
  cout << context << endl;
  cout << '\n';
  if (win == true) {
    cout << "Congratulations! You have won. Hooray!" << endl;
  }
  if (lose == true) {
    cout << "Sorry, you have lost. Better luck next time!" << endl;
  }
  if (win == false && lose == false && num_choice != 0) {
    cout << "What would you like to do?" << endl;
    cout << '\n';
    for (size_t i = 1; i <= num_choice; i++) {
      cout << " " << i << ". " << choice[i - 1].choice_context << endl;
    }
  }
}
