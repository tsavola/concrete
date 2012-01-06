import concrete

def func0():
	return 5

def func1(value):
	return value + 2

x = 10
r = func0() + func1(x)
r = concrete.test(r, foo=80, bar="jep jep") + 1000
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

print("urlopen:" + repr(concrete.urlopen("http://example.org/")))
