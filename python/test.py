import sys

from concrete import *

def main():
	with Context() as context:
		context.load_source("example/test.py")
		snapshot = context.snapshot()

	print("Changing context")

	with Context(snapshot) as context:
		while context.executable:
			try:
				context.execute()
			except Error as e:
				print("Error:", e, file=sys.stderr)

if __name__ == "__main__":
	main()
