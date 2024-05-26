# Four Knights w/ Searching Algorithms
Testing A* and Branch-and-Bound against the Four Knights puzzle for a class project.

Puzzle goal:

![image](https://github.com/NotsoanoNimus/four-knights-searching/assets/31320277/2cd31904-972d-4d57-8892-c18d81e920dd)


# Sample Output
You can clearly see how the A* Search minimizes the combined cost estimate at each move.

In fact, it's kept consistently at its combined minimum cost of `f(x) = 16`.

```
=~=~= Four Knights Puzzle Simulator =~=~=

-- Initializing game board...
B.b
...
W.w

-- Game goal state...
w.W
...
b.B

-- Running A* Search for best solution...

Discovered new possible move:
..b
..B
W.w
Stats:	f(x) = 16	g(x) = 1	h(x) = 15
	Unseen board hash recorded: 1612550

Discovered new possible move:
..b
...
WBw
Stats:	f(x) = 16	g(x) = 1	h(x) = 15
	Unseen board hash recorded: 1687550



[ . . . ]
[ skipping ]
[ . . . ]
[ skipping ]
[ . . . ]



 === Selected Route w/ Cost 16 ===
w.W
..b
..B

Discovered new possible move:
..W
..b
.wB
Stats:	f(x) = 18	g(x) = 16	h(x) = 2
	Unseen board hash recorded: 709450

Discovered new possible move:
w..
W.b
..B
Stats:	f(x) = 18	g(x) = 16	h(x) = 2
	Unseen board hash recorded: 397254

Discovered new possible move:
w..
..b
.WB
Stats:	f(x) = 18	g(x) = 16	h(x) = 2
	Unseen board hash recorded: 631254

Discovered new possible move:
w.W
...
b.B
Stats:	f(x) = 16	g(x) = 16	h(x) = 0
	Unseen board hash recorded: 421954

Discovered new possible move:
w.W
B.b
...
Stats:	f(x) = 18	g(x) = 16	h(x) = 2
	Unseen board hash recorded: 6454

 === Selected Route w/ Cost 16 ===
wBW
...
b..

Discovered new possible move:
.BW
..w
b..
Stats:	f(x) = 18	g(x) = 16	h(x) = 2
	Unseen board hash recorded: 43830

Discovered new possible move:
.BW
...
bw.
Stats:	f(x) = 18	g(x) = 16	h(x) = 2
	Unseen board hash recorded: 343830

Discovered new possible move:
w.W
...
b.B
Stats:	f(x) = 16	g(x) = 16	h(x) = 0
	Unseen board hash recorded: 421954

Discovered new possible move:
wB.
...
bW.
Stats:	f(x) = 18	g(x) = 16	h(x) = 2
	Unseen board hash recorded: 265634

 === Selected Route w/ Cost 16 ===
w.W
...
b.B


========================================
Final game route (16 steps):
B.b
...
W.w

..b
..B
W.w

...
..B
Wbw

.W.
..B
.bw

bW.
..B
..w

bW.
...
B.w

.W.
..b
B.w

.W.
w.b
B..

.Ww
..b
B..

.W.
..b
Bw.

wW.
..b
B..

w..
..b
B.W

w..
W.b
B..

wB.
W.b
...

wB.
W..
b..

w..
W..
b.B

w.W
...
b.B


==*=*=*=*=*=*=*=*=*=*=*==
Nice! You won!!!
	Tree Expansions with A*: 76
	Time taken: 0.000471 seconds

```
