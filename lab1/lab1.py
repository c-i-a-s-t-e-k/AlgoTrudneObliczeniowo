import dimacs
from itertools import *

# graph list
graphs = [
("e5"),
("e10"),
("e20"),
("e40"),
("e150"),
("s25"),
("s50"),
("s500"),
("b20"),
("b30"),
("b100"),
("k330_a"),
("k330_b"),
("k330_c"),
("m20"),
("m30"),
("m40"),
("m50"),
("m100"),
("p20"),
("p35"),
("p60"),
("p150"),
("r30_01"),
("r30_05"),
("r50_001"),
("r50_01"),
("r50_05"),
("r100_005"),
]

def bruteVertexCover(G):
    E = dimacs.edgeList(G)
    for k in range(len(G)):
        for C in combinations( range(len(G)), k ):
            C = set(C)
            flag = True
            for e in E:
                if not(e[0] in C or e[1] in C):
                    flag = False
                    break
            if flag:
                print(dimacs.isVC(E, C))
                return C
    return None

def _VC1(E, k, S, i):
    print(E[i], S)
    e = E[i]
    while e[0] in S and e[1] in S:
        i += 1
        e = E[i]
        if i >= len(E):
            return S
    
    if k == 0:
        return None
    
    S1 = _VC1(E, k-1, S.copy().add(e[0]), i + 1)
    S2 = _VC1(E, k-1, S.copy().add(e[1]), i + 1)

    if S1 is not None:
        return S1
    else:
        return S2
    

def rekurencion1VC(G):
    E = dimacs.edgeList(G)
    C = None
    for k in range(len(G)):
        C = _VC1(E, k, set(), 0)
        if C is not None: break
    return C





for name in graphs:
    # print(5)
    G = dimacs.loadGraph("graph/" + name)
    C = rekurencion1VC(G)
    if C is not None:
        dimacs.saveSolution("solution/rek1"+name, C)
        print
        continue
        