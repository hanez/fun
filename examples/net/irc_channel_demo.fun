#!/usr/bin/env fun

/*
 * IRC channel demo using lib/net/irc.fun
 *
 * Connects to an IRC server, registers, joins #funlang, sends a message to
 * the channel, reads a bit (responding to PINGs), then quits.
 *
 * TLS via local tunnel:
 *   - Point host to 127.0.0.1 and port to your local TLS tunnel.
 *   - Set use_tls = 1 (the library still uses plain TCP to the local tunnel).
 */

#include <net/irc.fun>
#include <strings.fun>
#include <io/console.fun>
#include <utils/datetime.fun>

// --- Configuration ---
// freenode (plain): host="chat.freenode.net", port=6667, use_tls=0
// For TLS via a local tunnel to remote 6697: host="127.0.0.1", port=<local>, use_tls=1
host = "chat.freenode.net"
port = 6667
use_tls = 0

nick = "FunChannelBot"     // Pick an unused nickname
user = "funchan"
real = "Hello superbrain! Fun IRC Channel Demo! Kisses hanez!"
pass = ""                   // Optional network password (not NickServ)
channel = "#fun-lang"        // Requested channel

// Date/time helper
dt = DateTime()

// Helper: read, answer PINGs, and print lines
fun handle_lines(c)
  lines = c.read_lines()
  i = 0
  while (i < len(lines))
    line = lines[i]
    msg = c.parse_message(line)
    if (msg["command"] == "PING")
      t = msg["trailing"]
      if (len(t) == 0 && len(msg["params"]) > 0)
        t = msg["params"][0]
      c.pong(t)
    print(line)
    i = i + 1

mode = " (plain)"
if (use_tls == 1)
  mode = " (TLS via tunnel)"
print("Connecting to " + host + ":" + to_string(port) + mode)
c = IRCClient(host, port, use_tls)
if (!c.connect())
  print("Failed to connect.")
  exit(1)

c.register(nick, user, real, pass)

// Process greetings and auto-respond to PINGs
start = dt.now_ms()
while (dt.now_ms() - start < 3000)
  handle_lines(c)

// Join #funlang and speak
c.join(channel)
dt.sleep_ms(500)
c.privmsg(channel, real)

// Read a bit more to display echoes/traffic
start2 = dt.now_ms()
while (dt.now_ms() - start2 < 2000)
  handle_lines(c)

c.quit("bye")
dt.sleep_ms(200)
c.close()

print("Done.")
