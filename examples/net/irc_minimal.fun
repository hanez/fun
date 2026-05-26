#!/usr/bin/env fun

/*
 * Minimal example using lib/net/irc.fun
 *
 * This demo connects to an IRC server, registers, joins a channel, sends one
 * message, reads for a short while (responding to PINGs), then quits.
 *
 * TLS usage via local tunnel (recommended):
 *   1) Install stunnel and create a client service (example in repo docs).
 *   2) Run this script pointing to localhost:<local_tls_port>, use_tls=1.
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

nick = "FunDemoBot"          // Pick an unused nickname
user = "fundemo"
real = "Fun Demo Bot"
pass = ""                     // Optional network password (not NickServ)
channel = "#funlang"           // Use a channel you may write to
target_nick = "hanez"         // For a test private message

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

// Join and speak
c.join(channel)
dt.sleep_ms(500)
c.privmsg(channel, "Hello from Fun!")

// Send a test message to a nick as well
c.privmsg(target_nick, "Hi from Fun example script!")

// Read a bit more
start2 = dt.now_ms()
while (dt.now_ms() - start2 < 2000)
  handle_lines(c)

c.quit("bye")
dt.sleep_ms(200)
c.close()

print("Done.")
