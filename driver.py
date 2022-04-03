import os
import sys

n = int(sys.argv[1])
p = int(sys.argv[2])

os.system("g++ client-phase1.cpp -o client-phase1 -lpthread")

for i in range(1,n+1):
    os.system("./client-phase{} client{}-config.txt files/client{} > output{} &".format(p,i,i,i))
