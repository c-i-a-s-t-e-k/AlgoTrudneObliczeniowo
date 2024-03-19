#!/usr/bin/python3

from copy import deepcopy
import sys

RERAISE = False


def getboard( board, x,y ):
  try:
    return board[y][x]
  except Exception as e:
    return "."


#  opqr  -- dziekan poziomy
# 
#  s     -- dziekan pionowy
#  t
#  u
#  v


def readBoard( fname ):
  f=open(fname)
  W,H,N = f.readline().strip('\n').split()
  W = int(W)
  H = int(H)
  N = int(N)
  board = []
  for i in range(H):
    line = f.readline().strip()
    line = [ d for d in line ]
    if len( line ) != W: raise Exception("Zla dlugosc wiersza w opisie planszy")
    board += [line]

  for y in range(H):
    for x in range(W):
      if getboard( board, x,y) != "o": continue
      above = getboard( board, x, y-1 )
      if above == "o":
        board[y-1][x] = "s"
        board[y][x] = "t"
      if above == "t": board[y][x] = "u"
      if above == "u": board[y][x] = "v"
      left = getboard( board, x-1, y )
      if left == "o": board[y][x] = "p"
      if left == "p": board[y][x] = "q"
      if left == "q": board[y][x] = "r"
    
  return W,H,N, board
    
    
def readSolution( fname ):
  sol = []
  line = ""
  f = open(fname)
  try:
    M = f.readline().strip('\n').split()[0]
    M = int(M)
  except Exception as e:
    raise Exception(f"Problem w pierwszej linii rozwiazania: {line}")  
  num = 1
  
  for i in range(M):
    num += 1
    line = f.readline()
    try:
      X,Y,D,L = line.strip('\n').split()
      X,Y,L = int(X), int(Y), int(L)
    except Exception as e:
      raise Exception(f"Problem w opisie rozwiazania, linia {num}: {line}")
    if X < 0 or Y < 0 or L < 0: raise Exception(f"Liczba ujemna w rozwiazaniu, linia {num}: {line}")
    if D not in "UDLR":         raise Exception(f"Bledny kierunek w rozwiazaniu, linia {num}: {line}")
    sol.append( (X,Y,D,L) )

               
  return sol




def drawBoard( board ):
  for line in board:
    print( "".join(line))
  print()


left  = "bcdpqr"
up    = "yzwtuv"
frnt = "axos"


TOFRNT = dict()
TOFRNT.update(  { d:(0,0) for d in frnt } )
TOFRNT.update(  { d:(-1,0) for d in left } )
TOFRNT.update(  { d:(0,-1) for d in up } )

PREV = {"b":"a", "c":"b", "d":"c",
        "y":"x", "z":"y", "w":"z",
        "p":"o", "q":"p", "r":"q",
        "t":"s", "u":"t", "v":"u" }
        
DIR = {"U": (0,-1), "D":(0,1), "L":(-1,0), "R":(1,0) }

 
def carfront( board, x, y ):
 
  while True:
    dx,dy = TOFRNT[ board[y][x] ]
    if (dx,dy) == (0,0): return x,y
    x += dx
    y += dy

def carend( board, x, y ):
  prev = board[y][x]
  if board[y][x] in "abcdoprq":
    dx, dy = (1,0)
  if board[y][x] in "xyzwstuv":
    dx, dy = (0,1)

  while True:
    x += dx
    y += dy
    ch = getboard( board, x, y )
    if ch in frnt or ch == "#" or ch == "." or prev != PREV[ch]:
      return x-dx, y-dy
    prev = ch
    
    


def singleMove( board, sx,sy, ex, ey, dx, dy, move_num ):
  if (dx == 1 or dy == 1) and getboard( board, ex+dx, ey+dy ) != ".": raise Exception( f"Bledny ruch {move_num}")
  if (dx == -1 or dy == -1) and getboard( board, sx+dx, sy+dy ) != ".": raise Exception( f"Bledny ruch {move_num}")
    
  if dx == -1 or dy == -1:
    x,y,fx,fy = sx,sy,ex,ey
  else:
    x,y,fx,fy = ex,ey,sx,sy

  while True:
    board[y+dy][x+dx] = board[y][x]
    board[y][x] = "."
    x -= dx
    y -= dy
    if x+dx == fx and y+dy == fy: return
    
        

def makeMove( board, W, H, move, move_num ):
  x,y,d,l = move

  if y < 0 or y >= H or x < 0 or x >= W:
    raise Exception(f"ruch {move_num} : {x} {y} {d} {l} jest poza plansza")

  if board[y][x] not in left and board[y][x] not in up and board[y][x] not in frnt:
    raise Exception(f"ruch {move_num} : {x} {y} {d} {l} nie dotyczy samochodu")

  
  sx,sy = carfront( board, x, y )
  ex,ey = carend( board, x, y )
  dean = False
  if board[sy][sx] == "o" or board[sy][sx] == "s":
    dean = True
  dx,dy = DIR[d]
  for s in range(l):
    if (sx+dx < 0 or sy+dy < 0 or ex+dx >= W or ey+dy >= H) and dean: return True
    singleMove( board, sx,sy, ex,ey, dx, dy, move_num )
    sx,sy,ex,ey = sx+dx, sy+dy, ex+dx, ey+dy
  return False
  


  





if __name__=="__main__":
  if len(sys.argv)<3:
    print("Wywołanie:\n   judge.py <infile> <outfile> [-v]")
  else:
    view = len(sys.argv)==4 and sys.argv[3]=='-v'

    try:
      W,H,N,board = readBoard(sys.argv[1])
      steps = readSolution(sys.argv[2] )
      if view: drawBoard( board )

      if len(steps) > N: raise Exception("za duzo krokow w rozwiazaniu")

      done = False
      for move_num,move in enumerate(steps):
        done = makeMove( board, W, H, move, move_num+1 )
        if view: drawBoard( board )
        if done: break

      if done:
        print('OK')
        exit(0)
      else:
        raise Exception("rozwiazanie nie doprowadzilo do wyjazdu dziekana")
        exit(1)
    except Exception as e:
      print("WRONG")
      print(e)
      if RERAISE: raise e
      exit(1)
