#!/bin/bash

# acutest
curl https://raw.githubusercontent.com/mity/acutest/master/include/acutest.h > acutest.h

# build test
# gcc -o fs_test fs_test.c -I./../src
gcc -std=c99 -o fs_test fs_test.c -I./../src

# run the tests
echo "testing filesystem..."
./fs_test

# cleanup
rm -f fs_test acutest.h