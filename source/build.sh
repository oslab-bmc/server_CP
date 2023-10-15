#!/bin/sh
#
cmake CMakeLists.txt
make
ssh-keygen -R "203.253.25.207"
scp ./output/bin/server_CP root@203.253.25.207:/tmp/