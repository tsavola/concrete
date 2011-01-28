import concrete

def func0():
	return 5

def func1(value):
	return value + 2

x = 10
r = func0() + func1(x)
r = concrete.test(r, foo=80, bar="jep jep") + 1000
