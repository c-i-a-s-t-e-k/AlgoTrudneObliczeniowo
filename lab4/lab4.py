import dimacs
import copy
from os import listdir
from os.path import isfile, join
from time import time
import copy

def simplifyClause( C:list, V:dict):
  # C - klauzula, czyli lista literałów
  # V - wartościowanie zmiennych
    if C == []:
        return C
    
    ans = []
    for i in C:
        if i not in V: ans.append(i)
        elif V[i]*i > 0:
            return None
        elif V[i]*i < 0:
            continue
    return ans

def simplifyCNF( CNF, V ):
  # CNF - formuła do uproszczenia
  # V   - wartościowanie zmiennych
    ans = []
    for clausure in CNF:
        if clausure is None:
            continue
        elif clausure == []:
            return None
        else:
            ans.append(simplifyClause(clausure, V))
    return ans


def solve( CNF, V:dict):
    # CNF to rozważana formuła
    # V to wartościowanie zmiennych
    if CNF is None:
        return V
    CNF = simplifyCNF(CNF, V)
    if CNF is None:
        return V
    elif CNF == []:
        return "UNSAT"
    
    v = None
    for clausure in CNF:
        if clausure is None:
            continue
        for i in clausure:
            if i not in V.keys():
                v = i
                break
        if v is not None: break
    
    V_cp = copy.deepcopy(V)
    V_cp[v] = 1
    V_cp[-v] = -1
    V_res = solve( CNF, V_cp )
    print(V_res)
    if V_res != "UNSAT":
        return V_cp
    V_cp[v] = -1
    V_cp[-v] = 1
    V_res = solve( CNF, V_cp )
    print(V_res)

    if V_res != "UNSAT":
        return V_cp

    return "UNSAT"

def zad1(filename):
    _, CNF = dimacs.loadCNF(filename)
    V = dict()
    return solve(CNF, V)


class Colors:
    RED = '\033[91m'
    GREEN = '\033[92m'
    YELLOW = '\033[93m'
    BLUE = '\033[94m'
    MAGENTA = '\033[95m'
    CYAN = '\033[96m'
    RESET = '\033[0m'

def test1():
    mypath = "lab4/sat"
    onlyfiles = [f for f in listdir(mypath) if isfile(join(mypath, f))]
    test_files = []
    for f in onlyfiles:
        if f[:2] == "5.":
            test_files.append(f)
        elif f[1:3] == "0." and f[1] in {"1", "2", "3"}:
            test_files.append(f)
            
    num_of_good = 0

    for f in test_files:
        print(f"working on:{f}", end="")
        f = join(mypath, f)
        start_time = time()
        res = zad1(f)
        all_time = time() - start_time
        if "yes" in f and res:
            print(f"{Colors.GREEN} good {Colors.BLUE}{all_time:.4f}{Colors.RESET}", end="")
            num_of_good += 1
        elif "no" in f and not res:
            print(f"{Colors.GREEN} good {Colors.BLUE}{all_time:.4f}{Colors.RESET}", end="")
            num_of_good += 1
        else:
            print(f"{Colors.RED} wrong {Colors.BLUE}{all_time:.4f}{Colors.RESET}", end="")
        print(f"\tres:{res}")
        

    print(len(test_files), num_of_good)

test1()
