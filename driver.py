import os
import sys

n = int(sys.argv[1])
p = int(sys.argv[2])

for i in range(1,n):
    os.system("./client-phase{} client{}-config.txt files/client{} > output{} &".format(p,i,i,i))

os.system("./client-phase{} client{}-config.txt files/client{} > output{}".format(p,n,n,n))
