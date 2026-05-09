# FLAGS for the project

cxx.flags :=
cxx.flags += -std=c++20
cxx.flags += -Wall -Wextra
cxx.flags += -g -O3
cxx.flags += -fno-semantic-interposition -fstrict-aliasing
cxx.flags += -fno-fast-math


ldxx.flags := -lncursesw -lmpfr -lreadline -lm -pthread

