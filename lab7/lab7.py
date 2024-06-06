from dimacs import *
from itertools import *
# for s in range(len(S)+1):
#    for c in combinations( A, s ):
#       print(set(c))

graphName = "graphtw/e5"

G = loadGraph(graphName  + ".gr")
B = loadDecomposition( graphName + ".tw")

print(G)
print()
print(B)

def checkVC(G, X, Y):
    n = len(G)
    covered = [False in range(n)]
    E = edgeList(G)
    for v,w in E:
        if v in X or w in X:
            covered[v] = True
            covered[w] = True
    for y in Y:
        if not covered[y]: return False
    return True


y_arr = []
C_arr = []

F = [[None for _ in y_arr] for _ in C_arr]

def get_C_id(C):
    raise "not implemented"

def f(y_id, C_id):
    if(F[y_id][C_id] != None):
        return F[y_id][C_id]
    y = y_arr[y_id]
    C = C_arr[C_id]
    
    if(checkVC(G, C, y.bag())):
        minmal = len(C)
        for z in y.children():            
            D = B[z].bag() - y.bag()
            minmal = min(minmal, f(z, get_C_id(D)))
        F[y_id][C_id] = len(C) + minmal
    else:
        F[y_id][C_id] = float("inf")



    

