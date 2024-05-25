from pulp import *
from dimacs import loadGraph, edgeList

def test_model():
    # stwórz model problemu o nazwi e test, minimalizacja funkcji celu
    model = LpProblem( "test", LpMinimize)   # LpMaximize dla maksymalizowania funkcji celu

    # stwórz trzy zmienne, x, y, z
    x = LpVariable( "x" , lowBound = 0, upBound = 5, cat = "Continuous" ) # zmienna ciągła z przedzialu [0,5]
    y = LpVariable( "y" , lowBound = 0, upBound = 3, cat = "Integer" )    # zmienna całkowita ze zbioru {0,1,2,3}
    z = LpVariable( "z" , cat = "Binary" )                                # zmienna binarna, wartosci ze zbioru {0,1}

    # dodaj funkcję celu do modelu
    model += 5-x+2*y-z    

    # dodaj ograniczenia (konieczne do spełnienia nierówności liniowe)
    model += x >= y
    model += y >= z

    print( model )


def vertex_cover_model(name):
    model = LpProblem("VertexCover", LpMinimize)
    x = dict()
    # x = LpVariable( "x", cat = "Binary")
    G = loadGraph(name)
    E = edgeList(G)

    for ei, ej in E:
        if not ei in x:
            x[ei] = LpVariable( f"x_{ei}", cat = "Binary")
        if not ej in x:
            x[ej] = LpVariable( f"x_{ej}", cat = "Binary")
        model += x[ei] + x[ej] >= 1

    model += sum(x.values())

    print(model)

    model.solve()                      # rozwiąż używając CBC
    print( LpStatus[model.status] )    # wypisz status rozwiązania

    # wypisz zmienne
    for var in model.variables():
        print( var.name, "=", var.varValue )

    # wypisz wartość funkcji celu
    print( value(model.objective) )

def weighted_vertex_cover_model(name):
    model = LpProblem("VertexCover", LpMinimize)
    x = dict()
    # x = LpVariable( "x", cat = "Binary")
    G = loadGraph(name)
    E = edgeList(G)

    for ei, ej in E:
        if not ei in x:
            x[ei] = LpVariable( f"x_{ei}", cat = "Binary")
        if not ej in x:
            x[ej] = LpVariable( f"x_{ej}", cat = "Binary")
        model += x[ei] + x[ej] >= 1

    fun = 0
    for i in x.keys():
        fun += len(G[i]) * x[i]
    model += fun

    print(model)

    model.solve()                      # rozwiąż używając CBC
    print( LpStatus[model.status] )    # wypisz status rozwiązania

    # wypisz zmienne
    for var in model.variables():
        print( var.name, "=", var.varValue )

    # wypisz wartość funkcji celu
    print( value(model.objective) )

def color_graph_model(name):
    model = LpProblem("VertexCover", LpMinimize)
    x = dict()
    # x = LpVariable( "x", cat = "Binary")
    G = loadGraph(name)
    K = 10
    E = edgeList(G)
    n = len(G) - 1
    x = dict()
    y = dict()

    # definiowanie zmiennych
    for i in range(1, n+1):
        for j in range(K):
            x[(i,j)] = LpVariable( f"x_{i},{j}" , cat = "Binary" )

    for j in range(K):
        y[j] = LpVariable( f"y_{j}" , cat = "Binary" )

    # każdy wierzchołek dokładnie jeden kolor
    for i in range(1, n+1):
        col_sum = 0
        for j in range(K):
            col_sum += x[(i,j)]
        model += col_sum == 1
    
    # sąsiednie wierzchołki dwa różne kolory
    for vi, vj in E:
        for j in range(K):
            model += (x[vi,j] + x[vj, j]) <= 1
    
    # jak wyższy kolor użyty to niższy też jest
    for j in range(K-1):
        model += y[j+1] <= y[j]


    for i in range(1, n+1):
        for j in range(K):
            model += x[(i,j)] <= y[j]
    
    model += sum(y.values())


    print(model)

    model.solve()                      # rozwiąż używając CBC
    print( LpStatus[model.status] )    # wypisz status rozwiązania

    # wypisz zmienne
    for var in model.variables():
        if(var.varValue == 1):
            print( var.name, "=", var.varValue )

    # wypisz wartość funkcji celu
    print( value(model.objective) )


color_graph_model("graph/k330_a")


