import tap

for i in range(2, 12, 3):
	print(i)

def func0():
	return 5

def func1(value):
	return value + 2

x = 10
r = func0() + func1(x)
print("r =", r)

def passthrough(x):
	return x

while x:
	print("foo")
	x = 0

s = "hello" + passthrough(" ") + "world"
print(repr(s), len(s))
print("id(None) =", id(None))
print(None, 1234.5 + passthrough(6.78), None)

print("urlopen:" + str(tap.urlopen("http://tsavola.github.com/concrete/")))

tap.execute(tap.urlopen, "http://tsavola.github.com/concrete/doxygen.png")
tap.execute(tap.urlopen, "http://tsavola.github.com/concrete/graph_legend.html")
