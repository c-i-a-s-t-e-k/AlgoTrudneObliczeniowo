import lab3.dimacs as dimacs
import os


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
("k330_d"),
("k330_e"),
("k330_f"),
("f30"),
("f35"),
("f40"),
("f56"),
("m20"),
("m30"),
("m40"),
("m50"),
("m100"),
("p20"),
("p35"),
("p60"),
("p150"),
("p200"),
("r30_01"),
("r30_05"),
("r50_001"),
("r50_01"),
("r50_05"),
("r100_005"),
("r100_01"),
("r200_001"),
("r200_005")
]


def approx2GraphCover(G):
    E = dimacs.edgeList(G)
    C = set()

    for e in E:
        if not(e[0] in C or e[1] in C):
            C.update(e)
    
    if dimacs.isVC(E, C):
        return C
    
def approxLogCover(G):
    n = len(G)
    V = sorted(range(n), key=lambda x:-len(G[x]))
    vRank = [len(G[v]) for v in V]
    vis = n
    C = set()
    while not V[0] is -float("inf"):
        v = V[0]
        vis -= 1
        vRank[v] = -float("inf")
        if (len(G[v]) > 0):
            C.add(v)
        for w in G[v]:
            vRank[w] -= 1
        V = sorted(V, key=lambda x:-vRank[x])

                
    
    if dimacs.isVC(dimacs.edgeList(G), C):
        return C
    # else:
        # print(C)
        # for a, b in dimacs.edgeList(G):
        #     if not(a in C or b in C):
        #         print(a,b)



for name in graphs:
    # print(5)
    G = dimacs.loadGraph("graph/" + name)
    C1 = approx2GraphCover(G)
    C2 = approxLogCover(G)
    if name == "e1=":
        print(C2)

    print(f"for graph {name}, edges:  alg1 vs alg2: :{len(C1)} : {len(C2)}")
