#!/usr/bin/env fun

/*
 * IRC Echo Bot example using lib/net/irc.fun
 *
 * Connects to an IRC server, registers, joins #funlang, and listens for
 * messages. If a message contains "!echo <text>", the bot replies with <text>.
 *
 * Notes:
 *  - Works over plain TCP. For TLS, point host/port to a local TLS tunnel
 *    (e.g., stunnel) and set use_tls=1.
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

bot_nick = "FunEchoBot"   // Choose an unused nickname
bot_user = "funecho"
bot_real = "Fun Echo Bot"
bot_pass = ""             // Optional network password (not NickServ)
channel = "#fun-lang"

// Globals used by the callback
g_client = nil
g_dt = DateTime()

// Extract nick from a prefix of form "nick!user@host" or just "nick"
fun prefix_nick(prefix)
  p = to_string(prefix)
  ex = find(p, "!")
  if (ex >= 0)
    return substr(p, 0, ex)
  at = find(p, "@")
  if (at >= 0)
    return substr(p, 0, at)
  return p

// Callback for IRCClient.pump(line, msg)
fun on_line(line, msg)
  // Print all received lines to console
  print(line)

  cmd = msg["command"]
  if (cmd == "PRIVMSG")
    params = msg["params"]
    trg = ""
    if (len(params) > 0)
      trg = params[0]
    txt = msg["trailing"]

    // Decide reply target: channel or private
    reply_to = trg
    if (str_to_upper(trg) == str_to_upper(bot_nick))
      reply_to = prefix_nick(msg["prefix"]) // PM -> reply to user

    // Detect command pattern: !echo <text>
    if (len(txt) >= 6)
      if (substr(txt, 0, 6) == "!echo ")
        echo_text = substr(txt, 6, len(txt) - 6)
        if (g_client != nil)
          g_client.privmsg(reply_to, echo_text)

// --- Main flow ---
mode = " (plain)"
if (use_tls == 1)
  mode = " (TLS via tunnel)"
print("Connecting to " + host + ":" + to_string(port) + mode)

c = IRCClient(host, port, use_tls)
if (!c.connect())
  print("Failed to connect.")
  exit(1)

g_client = c

c.register(bot_nick, bot_user, bot_real, bot_pass)

// Process greetings and auto-respond to PINGs via pump()
start = g_dt.now_ms()
while (g_dt.now_ms() - start < 3000)
  c.pump(on_line)

// Join the channel and announce
c.join(channel)
g_dt.sleep_ms(500)
c.privmsg(channel, "FunEchoBot online. Say: !echo <your text>")

// Stay connected and keep handling messages until interrupted
print("Listening (press Ctrl-C to quit)...")
while (true)
  c.pump(on_line)
  // avoid busy loop when there's no data
  g_dt.sleep_ms(50)
