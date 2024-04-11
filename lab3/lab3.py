import random         # biblioteka liczb pseudolosowych
import matplotlib.pyplot
import pycosat
import matplotlib

import dimacs
import copy
from os import listdir
from os.path import isfile, join
from time import time
# n = 5
# V = range(1,n+1)      # lista zmiennych 1...n
# x = random.choice(V)*random.choice(S)  # losowo wybrana zmienna z losowym negowaniem     
# print( x )

def zad1(k, n, T):
    S = [1,-1]            # lista +/-
    V = list(range(1, n+1))
    x = []
    y = []
    for i in range(1, 100):
        a = i / 10
        s = 0
        for j in range(T):
            formule = []
            for _ in range(int(n*a)):
                formule.append([random.choice(V)*random.choice(S) for _ in range(3)])
            sol = pycosat.solve(formule)
            if sol != u'UNSAT':
                s += 1
        print(".",end="")
        x.append(a)
        y.append(s/T)
    print()
    matplotlib.pyplot.plot(x, y)
    matplotlib.pyplot.xlabel("a")
    matplotlib.pyplot.ylabel("prob")
    matplotlib.pyplot.show()


zad1(3, 10, 100)

class Colors:
    RED = '\033[91m'
    GREEN = '\033[92m'
    YELLOW = '\033[93m'
    BLUE = '\033[94m'
    MAGENTA = '\033[95m'
    CYAN = '\033[96m'
    RESET = '\033[0m'

def zad2(path):
    n , x3c = dimacs.loadX3C(path)
    x = [0 for _ in x3c]
    # print(x3c)
    elements = [i for i in range(1,n+1)]
    cnf = [[] for i in range(n)]
    for i, S in enumerate(x3c):
        for element in S:
            cnf[element-1].append(i+1)
    for clauzure in copy.deepcopy(cnf):
        for i in range(len(clauzure)):
            for j in range(i+1, len(clauzure)):
                cnf.append([-clauzure[i], -clauzure[j]])

    sol = pycosat.solve(cnf)
    return sol != u'UNSAT'

def test_zad2():
    mypath = "lab3/x3c"
    onlyfiles = [f for f in listdir(mypath) if isfile(join(mypath, f))]
    num_of_good = 0

    for f in onlyfiles:
        print(f"working on:{f}", end="")
        f = join(mypath, f)
        start_time = time()
        res = zad2(f)
        all_time = time() - start_time
        if "yes" in f and res:
            print(f"{Colors.GREEN} good {Colors.BLUE}{all_time:.4f}{Colors.RESET}")
            num_of_good += 1
        elif "no" in f and not res:
            print(f"{Colors.GREEN} good {Colors.BLUE}{all_time:.4f}{Colors.RESET}")
            num_of_good += 1
        else:
            print(f"{Colors.RED} wrong {Colors.BLUE}{all_time:.4f}{Colors.RESET}")
        

    print(len(onlyfiles), num_of_good)        

test_zad2()

# Print text in various colors
# print(f"{Colors.RED}This is red!{Colors.RESET}")
# print(f"{Colors.GREEN}This is green!{Colors.RESET}")
# print(f"{Colors.YELLOW}This is yellow!{Colors.RESET}")
# print(f"{Colors.BLUE}This is blue!{Colors.RESET}")
# print(f"{Colors.MAGENTA}This is magenta!{Colors.RESET}")
# print(f"{Colors.CYAN}This is cyan!{Colors.RESET}")

