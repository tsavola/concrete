import http.client
import http.server
import struct
import sys

from concrete import *

def main():
	if len(sys.argv) > 1:
		port = int(sys.argv[1])
		server(("", port))
	else:
		client()

def client():
	with Context() as context:
		context.load_source("python/forkcode.py")

		while context.executable:
			try:
				context.execute()
			except Error as e:
				print("Error:", e, file=sys.stderr)

class Server(http.server.HTTPServer):

	allow_address_reuse = True

class Handler(http.server.BaseHTTPRequestHandler):

	def do_POST(self):
		size = int(self.headers["content-length"])
		data = self.rfile.read(size)

		with Context(data) as context:
			while context.executable:
				try:
					context.execute()
				except Error as e:
					print("Error:", e, file=sys.stderr)

		self.send_response(http.client.OK)
		self.send_header("Content-Length", 0)
		self.end_headers()

def server(address):
	Server(address, Handler).serve_forever()

if __name__ == "__main__":
	main()
