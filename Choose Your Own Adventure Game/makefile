CPPFLAGS=-ggdb3 -Wall -Werror -pedantic -std=gnu++98
PROGS=cyoa-step1 cyoa-step2 cyoa-step3 cyoa-step4
all: $(PROGS)
cyoa-step1:cyoa-step1.cpp page.hpp page.cpp
	g++ $(CPPFLAGS) -o cyoa-step1 cyoa-step1.cpp page.cpp
cyoa-step2:cyoa-step2.cpp story.hpp story.cpp page.hpp page.cpp
	g++ $(CPPFLAGS) -o cyoa-step2 cyoa-step2.cpp story.cpp page.cpp
cyoa-step3:cyoa-step3.cpp story.hpp story.cpp page.hpp page.cpp
	g++ $(CPPFLAGS) -o cyoa-step3 cyoa-step3.cpp story.cpp page.cpp
cyoa-step4:cyoa-step4.cpp story.hpp story.cpp page.hpp page.cpp
	g++ $(CPPFLAGS) -o cyoa-step4 cyoa-step4.cpp story.cpp page.cpp
