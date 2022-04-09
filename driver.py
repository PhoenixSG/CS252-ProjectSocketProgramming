import os
import sys

n = int(sys.argv[1])
p = int(sys.argv[2])

os.system("g++ client-phase1.cpp -o client-phase1 -lpthread")

for i in range(1,n):
    os.system("./client-phase{}-threaded client{}-config.txt files/client{} output{}s > output{}&".format(p,i,i,i,i))

os.system("./client-phase{}-threaded client{}-config.txt files/client{} output{}s > output{}".format(p,n,n,n,n))
