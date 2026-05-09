# FLAGS for the project

cxx.flags :=
cxx.flags += -std=c++20
cxx.flags += -Wall -Wextra -Wstrict-aliasing -ferror-limit=1
cxx.flags += -g -O3
cxx.flags += -fno-semantic-interposition -fstrict-aliasing
cxx.flags += -fno-fast-math
cxx.flags += -mfma -mavx2 -mrdrnd
cxx.flags += -flto=thin


ldxx.flags := -lncursesw -lmpfr -lreadline -lm -pthread -flto=thin

