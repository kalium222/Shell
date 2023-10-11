#!/bin/bash

if [ "$1" == "-s" ]; then
    gcc shell.c command.c built-in.c shell_errors.c
    ./a.out
    exit
fi

cpplint --linelength=120 --filter=-legal,-readability/casting,-whitespace,-runtime/printf,-runtime/threadsafe_fn,-readability/todo,-build/include_subdir,-build/header_guard *.h *.c
cppcheck --enable=all *.h *.c
cd build/
make
cd ..

if [ "$1" == "-r" ]; then
    ./build/shell
elif [ "$1" == "-m" ]; then
    # valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./build/shell
    valgrind --leak-check=full ./build/shell
elif [ "$1" == "-j" ]; then
    git add .
    git commit --allow-empty -m "joj. $2"
    git push
elif [ "$1" == "-p" ]; then
    zip shell CMakeLists.txt shell.c command.c command.h built-in.c built-in.h shell_errors.c shell_errors.h
    # scp shell.zip root@
else
    echo complete
    echo "Usage: ./test.sh [-s] [-r] [-m] [-j] [-p]"
fi


