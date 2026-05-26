#!/usr/bin/env fun

/*
 * Fully interactive IRC client example using lib/net/irc.fun
 *
 * This script lets you interact with an IRC server from the terminal:
 * - Shows incoming messages (processed between your inputs)
 * - Lets you type slash-commands similar to classic IRC clients
 *
 * Notes/limitations:
 * - The Fun stdlib provides blocking input(); there is no stdin polling.
 *   We therefore alternate: process network I/O, then read one user line.
 *   Server PINGs are handled whenever we process network I/O (between inputs).
 * - For TLS, point host/port to a local TLS tunnel (e.g., stunnel) and set use_tls=1.
 */

#include <net/irc.fun>
#include <strings.fun>
#include <io/console.fun>
#include <io/process.fun>
#include <utils/datetime.fun>

// --- Configuration ---
// freenode (plain): host="chat.freenode.net", port=6667, use_tls=0
// For TLS via a local tunnel to server:6697 -> host="127.0.0.1", port=<local>, use_tls=1
host = "chat.freenode.net"
port = 6667
use_tls = 0

default_nick = "FunUser"
default_user = "funuser"
default_real = "Fun Interactive Client"
network_pass = ""  // Optional network password (not NickServ)

// Optionally auto-join a channel after connect (empty string to skip)
auto_join = "#fun-lang"

dt = DateTime()
proc = Process()

// Initialize globals used by prompt/printing early so handlers can access them
current_chan = ""
current_nick = default_nick
// When set to 1, we skip the pre-prompt drain and immediately show the prompt
want_fast_prompt = 0
// Small counter to skip a few network pump cycles right after commands
skip_pump_ticks = 0

// --- Timing knobs (tunable at runtime) ---
// How long to drain incoming lines before each prompt (ms). 0 => no drain.
pre_drain_ms = 0
// Sleep between empty pumps during the pre-prompt drain (ms)
pre_drain_idle_ms = 20
// After sending a line (/msg or plain), how long to briefly pump (ms)
send_pump_ms = 400

// --- Helpers ---
fun print_help()
  print("Commands:")
  print("  /help                      Show this help")
  print("  /nick <newnick>            Change nickname")
  print("  /join <#channel>           Join a channel")
  print("  /part [#channel] [msg]     Leave a channel (current if omitted)")
  print("  /msg <target> <text>       Send PRIVMSG to channel or user")
  print("  /me  <target> <text>       Send ACTION (/me) to target")
  print("  /raw <line>                Send a raw IRC line")
  print("  /names [#channel]          Show names for a channel")
  print("  /drainms <ms>              Set pre-prompt drain milliseconds (0 = off)")
  print("  /sendpump <ms>             Set brief post-send pump ms")
  print("  /showtimers                Show current timing settings")
  print("  /quit [message]            Quit and exit")
  print("")
  print("Typing a line without '/' will send it to the current channel if set.")

// Extract nick from a prefix like "nick!user@host"
fun prefix_nick(prefix)
  p = to_string(prefix)
  ex = find(p, "!")
  if (ex >= 0)
    return substr(p, 0, ex)
  at = find(p, "@")
  if (at >= 0)
    return substr(p, 0, at)
  return p

// Internal helper: ensure params is an Array
fun _msg_params_safe(m)
  p = m["params"]
  if (typeof(p) == "Array")
    return p
  return []

// Render a parsed message in a readable way (defensive conversions)
fun render_msg(msg)
  cmd = to_string(msg["command"])
  if (cmd == "PRIVMSG")
    sender = prefix_nick(to_string(msg["prefix"]))
    ps = _msg_params_safe(msg)
    tgt_s = ""
    if (len(ps) > 0)
      tgt_s = to_string(ps[0])
    txt_s = to_string(msg["trailing"])
    if (len(tgt_s) > 0)
      // Avoid join() to prevent any accidental shadowing; use concatenation
      out = "[" + tgt_s + "] <" + sender + "> " + txt_s
      print(out)
    else
      out = "<" + sender + "> " + txt_s
      print(out)
    return
  if (cmd == "NOTICE")
    sender = prefix_nick(to_string(msg["prefix"]))
    ps = _msg_params_safe(msg)
    // target (unused in print below, but keep for parity with PRIVMSG)
    tgt_s = ""
    if (len(ps) > 0)
      tgt_s = to_string(ps[0])
    txt_s = to_string(msg["trailing"])
    out = "-" + sender + "- " + txt_s
    print(out)
    return
  if (cmd == "PING")
    // Handled by pump(); we don't spam output for PINGs
    return
  // Default: print raw line representation
  // (Caller prints the original line; this is an extra if wanted.)

// Build the prompt string reflecting current channel
fun _prompt_str()
  // Include current local date/time with seconds in the prompt for debugging
  ts = dt.format(dt.now_ms(), "%Y-%m-%d %H:%M:%S")
  p = "[" + ts + "] irc"
  if (len(current_chan) > 0)
    p = p + "[" + current_chan + "]"
  return p + "> "

// Repaint the prompt immediately (used after commands/output)
fun _repaint_prompt()
  // Force a fresh line, then print the prompt
  print("")
  echo(_prompt_str())

// Process incoming lines once; auto-PONG is done in IRCClient.pump via callback
fun on_line(line, msg)
  // Print raw lines only for maximum robustness (avoids CALL runtime issues)
  print(line)
  // Update local state based on parsed message so prompt/channel reflect reality
  cmd = to_string(msg["command"])
  ps = _msg_params_safe(msg)
  // Track our own NICK changes
  if (cmd == "NICK")
    oldn = prefix_nick(to_string(msg["prefix"]))
    newn = ""
    if (len(ps) > 0)
      newn = to_string(ps[0])
    else
      newn = to_string(msg["trailing"])  // fallback
    if (oldn == current_nick) 
      current_nick = newn
  // Track JOIN/PART to set/unset current_chan when it's us
  if (cmd == "JOIN")
    who = prefix_nick(to_string(msg["prefix"]))
    ch = ""
    if (len(ps) > 0)
      ch = to_string(ps[0])
    else
      ch = to_string(msg["trailing"])  // e.g., ":#chan"
    if (len(ch) > 0 && substr(ch, 0, 1) == ":")
      ch = substr(ch, 1, len(ch) - 1)
    if (who == current_nick)
      current_chan = ch
      print("Joined " + current_chan)
  if (cmd == "PART")
    who = prefix_nick(to_string(msg["prefix"]))
    ch = ""
    if (len(ps) > 0)
      ch = to_string(ps[0])
    if (who == current_nick)
      if (str_to_upper(ch) == str_to_upper(current_chan))
        current_chan = ""
  // Track numerics that confirm join/names
  if (cmd == "353" || cmd == "366")
    // Try to find a channel name among params
    ch = ""
    i = 0
    while (i < len(ps))
      p = to_string(ps[i])
      if (len(p) > 0 && substr(p, 0, 1) == "#")
        ch = p
        break
      i = i + 1
    if (len(ch) > 0)
      // If we had not set current channel yet, set it now
      if (len(current_chan) == 0)
        current_chan = ch
        print("Joined " + current_chan)
  // After any incoming line, re-echo the prompt so it's visible again
  // (helps after JOIN bursts)
  print("")
  echo(_prompt_str())

// Split one line into argv-like array by spaces, keeping trailing text after :
fun split_irc_cmd(line)
  s = str_trim(to_string(line))
  if (len(s) == 0)
    return []
  // If contains a space-colon, treat ":..." as a single arg
  sp = find(s, " :")
  if (sp >= 0)
    left = substr(s, 0, sp)
    right = substr(s, sp + 2, len(s) - sp - 2)
    parts = str_split(left, " ")
    push(parts, right)
    return parts
  return str_split(s, " ")

// Join parts[start..end] with single spaces
fun join_rest(parts, start)
  out = ""
  i = to_number(start)
  while (i < len(parts))
    if (len(out) == 0)
      out = to_string(parts[i])
    else
      out = out + " " + to_string(parts[i])
    i = i + 1
  return out

// Briefly pump the IRC client to process server I/O (e.g., after sending)
fun pump_brief(client, ms)
  duration = to_number(ms)
  if (duration <= 0)
    duration = 150
  t0 = dt.now_ms()
  while (dt.now_ms() - t0 < duration)
    n = client.pump(on_line)
    if (n == 0)
      dt.sleep_ms(30)

// Try to read one line from stdin non-blockingly using an external shell.
// Returns map { "ok": 1/0, "line": string }
fun _stdin_try_read(timeout_ms)
  ms = to_number(timeout_ms)
  if (ms < 0)
    ms = 0
  // Build bash command: read with timeout; on success print the line; exit 0
  // On timeout/EOF, exit non-zero with no output so we can distinguish.
  // Use -lc to ensure non-interactive login shell features are available.
  tsec = to_string(ms / 1000)
  // Keep a minimum of 0.02s to avoid tight CPU spin
  if (ms < 20)
    tsec = "0.02"
  cmd = "bash -lc 'read -t " + tsec + " -r line || exit 1; printf %s \"$line\"'"
  r = proc.run(cmd)
  if (r["code"] == 0)
    return { "ok": 1, "line": to_string(r["out"]) }
  return { "ok": 0, "line": "" }

// --- Main ---
mode = " (plain)"
if (use_tls == 1)
  mode = " (TLS via tunnel)"
print("Connecting to " + host + ":" + to_string(port) + mode)

c = IRCClient(host, port, use_tls)
if (!c.connect())
  print("Failed to connect.")
  exit(1)

// Register
c.register(default_nick, default_user, default_real, network_pass)

// Process initial greetings (and answer PINGs)
greet_start = dt.now_ms()
while (dt.now_ms() - greet_start < 2500)
  c.pump(on_line)

current_chan = ""
current_nick = default_nick
if (len(auto_join) > 0)
  c.join(auto_join)
  // Don't set current_chan yet; wait for server confirmation
  dt.sleep_ms(400)
  // Drain any immediate JOIN replies
  c.pump(on_line)

con = Console()
print_help()

// Show initial prompt once
print("")
echo(_prompt_str())

while true
  // 1) First, try to read any user input without blocking. This ensures
  //    commands execute immediately and are not delayed by network pumping.
  polled = _stdin_try_read(60)
  if (polled["ok"] == 1)
    cmdline = str_trim(polled["line"])
  else
    cmdline = ""

  if (len(cmdline) == 0)
    // 2) No user input this tick — optionally defer pumping briefly right
    //    after a command to keep the prompt responsive.
    if (skip_pump_ticks > 0)
      skip_pump_ticks = skip_pump_ticks - 1
      // keep the prompt visible
      echo("")
      dt.sleep_ms(10)
      continue
    // Then pump server traffic once. If the
    //    underlying recv() blocks until data, keep the sleep short to
    //    avoid busy-waiting between bursts.
    n = c.pump(on_line)
    if (n == 0)
      dt.sleep_ms(15)
    // keep looping
    continue

  if (substr(cmdline, 0, 1) == "/")
    // Slash command
    words = split_irc_cmd(substr(cmdline, 1, len(cmdline) - 1))
    if (len(words) == 0)
      continue
    op = str_to_lower(words[0])

    if (op == "help")
      print_help()
      _repaint_prompt()
      want_fast_prompt = 1
      skip_pump_ticks = 3
      // Skip immediate network pumping this iteration so the prompt
      // stays responsive right after command execution.
      continue

    if (op == "nick")
      if (len(words) < 2)
        print("Usage: /nick <newnick>")
      else
        c.send_raw("NICK " + words[1])
        current_nick = words[1]
      _repaint_prompt()
      want_fast_prompt = 1
      skip_pump_ticks = 3
      continue

    if (op == "join")
      if (len(words) < 2)
        print("Usage: /join <#channel>")
      else
        ch = words[1]
        c.join(ch)
        // Wait for server JOIN/353/366 to confirm; we'll update current_chan in on_line
        print("Joining " + ch)
      _repaint_prompt()
      want_fast_prompt = 1
      skip_pump_ticks = 3
      continue

    if (op == "part")
      // /part [#chan] [msg]
      ch = current_chan
      reason = ""
      if (len(words) >= 2)
        ch = words[1]
      if (len(words) >= 3)
        reason = words[2]
      if (len(ch) == 0)
        print("Usage: /part [#channel] [message]")
      else
        c.part(ch, reason)
        if (str_to_upper(ch) == str_to_upper(current_chan))
          current_chan = ""
      _repaint_prompt()
      want_fast_prompt = 1
      skip_pump_ticks = 3
      continue

    if (op == "msg")
      if (len(words) < 3)
        print("Usage: /msg <target> <text>")
      else
        tgt = words[1]
        txt = join_rest(words, 2)
        c.privmsg(tgt, txt)
      _repaint_prompt()
      want_fast_prompt = 1
      skip_pump_ticks = 3
      continue

    if (op == "me")
      if (len(words) < 3)
        print("Usage: /me <target> <text>")
      else
        tgt = words[1]
        txt = join_rest(words, 2)
        c.privmsg(tgt, "\x01ACTION " + txt + "\x01")
      _repaint_prompt()
      want_fast_prompt = 1
      skip_pump_ticks = 3
      continue

    if (op == "raw")
      if (len(words) < 2)
        print("Usage: /raw <line>")
      else
        c.send_raw(join_rest(words, 1))
      _repaint_prompt()
      want_fast_prompt = 1
      skip_pump_ticks = 3
      continue

    if (op == "drainms")
      if (len(words) < 2)
        print("Usage: /drainms <milliseconds>")
      else
        pre_drain_ms = to_number(words[1])
        print("pre_drain_ms set to " + to_string(pre_drain_ms) + " ms")
      _repaint_prompt()
      want_fast_prompt = 1
      skip_pump_ticks = 3
      continue

    if (op == "sendpump")
      if (len(words) < 2)
        print("Usage: /sendpump <milliseconds>")
      else
        send_pump_ms = to_number(words[1])
        print("send_pump_ms set to " + to_string(send_pump_ms) + " ms")
      _repaint_prompt()
      want_fast_prompt = 1
      skip_pump_ticks = 3
      continue

    if (op == "showtimers")
      print("Timers:")
      print("  pre_drain_ms      = " + to_string(pre_drain_ms))
      print("  pre_drain_idle_ms = " + to_string(pre_drain_idle_ms))
      print("  send_pump_ms      = " + to_string(send_pump_ms))
      _repaint_prompt()
      want_fast_prompt = 1
      skip_pump_ticks = 3
      continue

    if (op == "names")
      ch = current_chan
      if (len(words) >= 2)
        ch = words[1]
      if (len(ch) == 0)
        print("Usage: /names [#channel]")
      else
        c.send_raw("NAMES " + ch)
      _repaint_prompt()
      want_fast_prompt = 1
      continue

    if (op == "quit")
      msg = ""
      if (len(words) >= 2)
        msg = words[1]
      c.quit(msg)
      dt.sleep_ms(200)
      c.close()
      print("Goodbye.")
      break

    print("Unknown command. Type /help")
    _repaint_prompt()
    want_fast_prompt = 1
    skip_pump_ticks = 3
    continue

  // Regular message: send to current channel if set
  if (len(current_chan) == 0)
    print("No current channel. Use /join <#channel> or /msg <nick> <text>.")
    _repaint_prompt()
  else
    c.privmsg(current_chan, cmdline)
    // Local echo so you see what you sent immediately
    print("[" + current_chan + "] <" + current_nick + "> " + cmdline)
    // Repaint prompt after sending; server responses will print concurrently
    _repaint_prompt()
    want_fast_prompt = 1
    skip_pump_ticks = 3

// Exit program
