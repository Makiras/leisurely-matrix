import sys
import random
from time import *

if len(sys.argv) > 1:
     n = int(sys.argv[1])
else:
     n = 1024

print("Python matrix loop size: {}".format(n))

A = [[random.random()
      for row in range(n)]
     for col in range(n)]
B = [[random.random()
      for row in range(n)]
     for col in range(n)]
C = [[0 for row in range(n)]
     for col in range(n)]

start = time()
for i in range(n):
    for j in range(n):
        for k in range(n):
            C[i][j] += A[i][k] * B[k][j]
end = time()

print("%0.6f" % (end - start))
