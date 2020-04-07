# Computer Network - Project 2 2019/2020 - Tests

To install mscgen:
- sudo apt-get install -y mscgen

To create log-packets.so:
- gcc -shared -fPIC -Wall -Werror -O3 -o log-packets.so log-packets.c -ldl

To create msc.eps:
- ./run.sh
