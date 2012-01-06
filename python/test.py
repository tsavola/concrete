import fcntl
import hashlib
import os
import sys
import threading
import time

from concrete import *

def main():
	pipe = os.pipe()
	fcntl.fcntl(pipe[0], fcntl.F_SETFL, fcntl.fcntl(pipe[0], fcntl.F_GETFL) | os.O_NONBLOCK)

	t = threading.Thread(target=ticker, args=(pipe[1],))
	t.daemon = True
	t.start()

	with Context() as context:
		context.event_wait(pipe[0], EVENT_FILE_READABLE, python_callback)

		context.load_source("example/test.py")
		snapshot = context.snapshot()

	print("Changing context")

	with Context(snapshot) as context:
		context.event_wait(pipe[0], EVENT_FILE_READABLE, python_callback)

		while context.executable:
			try:
				context.execute()
			except Error as e:
				print("Error:", e, file=sys.stderr)

def ticker(fd):
	while True:
		os.write(fd, b"\0")
		time.sleep(0.1)

def python_callback(context, fd, data):
	snapshot = context.snapshot()

	md5 = hashlib.md5()
	md5.update(snapshot)

	print("python_callback: %r snapshot len=%d md5=%s" % (context, len(snapshot), md5.hexdigest()))

	try:
		os.read(fd, 1)
	except:
		pass

	return True

if __name__ == "__main__":
	main()
