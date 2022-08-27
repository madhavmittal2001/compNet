import argparse
import math
from numpy import random
import matplotlib.pyplot as plt 

MSS = 1
RWS = 1000

parser = argparse.ArgumentParser()

parser.add_argument("-i", default=1, type=float)
parser.add_argument("-m", default=1, type=float)
parser.add_argument("-n", default=0.5, type=float)
parser.add_argument("-f", default=0.1, type=float)
parser.add_argument("-s", default=0.99, type=float)
parser.add_argument("-T", default=1000, type=int)
parser.add_argument("-o", default="cw")

arg = parser.parse_args()

Ki = arg.i
Km = arg.m
Kn = arg.n
Kf = arg.f
Ps = arg.s
t_seg = arg.T
otpt = arg.o

random.seed(1)

cw_upd = []

output = open(otpt+".txt", 'w')

cw_new = Ki * MSS
ct = RWS/2
success_seg = 0 #number of segments till which ACK received without failure
round = 0

while(success_seg < t_seg):
    round += 1
    cw_old = cw_new
    # plt.scatter([len(cw_upd)],[cw_old],c="red",marker='.')
    i = 0
    while(i < math.ceil(cw_old)):
        cw_upd.append(cw_new)
        if(random.choice([0,1], p=[1-Ps, Ps])): # 0 selected with probability 1-Ps and 1 with probability Ps
            success_seg += 1
            if(cw_new < ct):
                cw_new = (min(cw_new + Km * MSS, RWS))
            else:
                cw_new = (min(cw_new + Kn * MSS * MSS/cw_new, RWS))
            i += 1
            if(success_seg == t_seg):
                break
        else:
            ct = (cw_new/2) 
            cw_new = (max(1, Kf * cw_new))
            while(i < math.ceil(cw_old)):
                random.choice([0,1], p=[1-Ps, Ps]) # 0 selected with probability 1-Ps and 1 with probability Ps
                # we dont care about whether these segments were acknowledged
                i += 1

for cw in cw_upd:
    output.write(str((cw)) + "\n")

output.close()

plt.scatter([], [], marker='.', c='white' , label=str(round) + " rounds")
plt.plot(range(1, len(cw_upd) + 1), cw_upd)
plt.xlabel("update number")
plt.ylabel("CW")
plt.title(f"$K_i=${Ki} $K_m=${Km} $K_n=${Kn} $K_f=${Kf} $P_s=${Ps}")
plt.legend()
plt.savefig(otpt + ".png")