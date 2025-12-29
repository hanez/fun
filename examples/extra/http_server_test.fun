#include <io/socket.fun>

c = TcpClient()
if (c.connect("127.0.0.1", 8080))
    print("Connecting to server...")
    // Test GET
    print("Testing GET...")
    c.send("GET /info.fun?foo=bar&baz=qux HTTP/1.1\r\nHost: localhost\r\n\r\n")
    resp = c.recv_all(4096)
    print("GET Response contains foo=bar: " + to_string(find(resp, "foo = bar") >= 0))
    print("GET Response contains baz=qux: " + to_string(find(resp, "baz = qux") >= 0))
    
    c.close()
else
    print("Failed to connect to server")

if (c.connect("127.0.0.1", 8080))
    // Test POST
    print("Testing POST...")
    body = "postfoo=postbar&postbaz=postqux"
    req = "POST /info.fun HTTP/1.1\r\n"
    req = req + "Host: localhost\r\n"
    req = req + "Content-Length: " + to_string(len(body)) + "\r\n"
    req = req + "\r\n"
    req = req + body
    c.send(req)
    resp = c.recv_all(4096)
    print("POST Response contains postfoo=postbar: " + to_string(find(resp, "postfoo = postbar") >= 0))
    print("POST Response contains postbaz=postqux: " + to_string(find(resp, "postbaz = postqux") >= 0))
    c.close()
else
    print("Failed to connect to server for POST")
