import tap

url = "http://localhost:12345/"

result = tap.fork(url)

print(repr(result))
