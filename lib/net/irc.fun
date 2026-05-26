/*
 * Basic IRC client library for Fun
 *
 * Implements a class-based client for the IRC protocol suitable for simple
 * bots or message senders. Designed to work over plain TCP. For TLS, point
 * the client at a local TLS tunnel (e.g., stunnel) that connects to the
 * remote IRC TLS endpoint.
 */

#include <io/socket.fun>
#include <strings.fun>

class IRCClient(string host, number port, number use_tls)

  fun _construct(this, host, port, use_tls)
    this.host = to_string(host)
    this.port = to_number(port)
    this.use_tls = to_number(use_tls)  // 0 = plain, 1 = TLS via local tunnel
    this.nick = ""
    this.user = ""
    this.realname = ""
    this.password = ""
    this.connected = 0
    this._buf = ""    // line buffer for recv()

    // Under current stdlib, only TcpClient is available. For TLS, connect to
    // a local tunnel (e.g., stunnel) so we still use plain TCP here.
    this.cli = TcpClient()

  fun connect(this)
    ok = this.cli.connect(this.host, this.port)
    if (ok)
      this.connected = 1
    else
      this.connected = 0
    return ok

  fun is_connected(this)
    return this.connected == 1 && this.cli.is_connected()

  fun close(this)
    if (this.cli != nil)
      this.cli.close()
    this.connected = 0
    return 1

  // Basic RFC 1459 registration; password optional (PASS before NICK/USER)
  fun register(this, nick, user, realname, password)
    this.nick = to_string(nick)
    this.user = to_string(user)
    this.realname = to_string(realname)
    this.password = to_string(password)

    if (len(this.password) > 0)
      this.send_raw("PASS " + this.password)

    this.send_raw("NICK " + this.nick)
    // USER <username> <mode: 0> <unused: *> :<realname>
    this.send_raw("USER " + this.user + " 0 * :" + this.realname)
    return 1

  // Send raw IRC line (no trailing CRLF required by the caller)
  fun send_raw(this, line)
    if (!this.is_connected())
      return -1
    // Ensure CRLF termination
    msg = to_string(line)
    // Avoid dependency on str_ends_with; do it manually
    need_crlf = 1
    if (len(msg) >= 2)
      tail = substr(msg, len(msg) - 2, 2)
      if (tail == "\r\n")
        need_crlf = 0
    if (need_crlf == 1)
      msg = msg + "\r\n"
    return this.cli.send(msg)

  fun join(this, channel)
    return this.send_raw("JOIN " + to_string(channel))

  fun part(this, channel, msg)
    ch = to_string(channel)
    m = to_string(msg)
    if (len(m) > 0)
      return this.send_raw("PART " + ch + " :" + m)
    return this.send_raw("PART " + ch)

  fun privmsg(this, target, text)
    t = to_string(text)
    return this.send_raw("PRIVMSG " + to_string(target) + " :" + t)

  fun notice(this, target, text)
    return this.send_raw("NOTICE " + to_string(target) + " :" + to_string(text))

  fun pong(this, token)
    return this.send_raw("PONG :" + to_string(token))

  fun quit(this, message)
    m = to_string(message)
    if (len(m) > 0)
      this.send_raw("QUIT :" + m)
    else
      this.send_raw("QUIT")
    return 1

  // --- Reading and parsing ---

  // Read any available data and emit complete IRC lines (without CRLF)
  fun read_lines(this)
    if (!this.is_connected())
      return []
    // Read a chunk (tune size as needed)
    chunk = this.cli.recv(4096)
    if (len(chunk) == 0)
      return []

    this._buf = this._buf + chunk
    out = []
    // Split on CRLF (IRC standard), also tolerate lone LF
    while true
      crlf = find(this._buf, "\r\n")
      nl = find(this._buf, "\n")
      sep = -1
      seplen = 0
      if (crlf >= 0)
        sep = crlf
        seplen = 2
      else
        if (nl >= 0)
          sep = nl
          seplen = 1

      if (sep < 0)
        break

      line = substr(this._buf, 0, sep)
      this._buf = substr(this._buf, sep + seplen, len(this._buf) - sep - seplen)
      // Strip any trailing CR or LF just in case
      line = str_trim(line)
      if (len(line) > 0)
        push(out, line)

    return out

  // Simple IRC message parser -> Map { prefix, command, params (Array), trailing }
  fun parse_message(this, line)
    s = to_string(line)
    n = len(s)

    prefix = ""
    if (n > 0 && substr(s, 0, 1) == ":")
      sp = find(s, " ")
      if (sp > 1)
        prefix = substr(s, 1, sp - 1)
        s = substr(s, sp + 1, len(s) - sp - 1)

    // Command up to first space or EOL
    sp2 = find(s, " ")
    command = ""
    rest = ""
    if (sp2 >= 0)
      command = substr(s, 0, sp2)
      rest = str_trim(substr(s, sp2 + 1, len(s) - sp2 - 1))
    else
      command = s
      rest = ""

    // Params until a " :trailing" token; trailing consumes remainder
    params = []
    trailing = ""
    while len(rest) > 0
      if (substr(rest, 0, 1) == ":")
        trailing = substr(rest, 1, len(rest) - 1)
        break
      sp = find(rest, " ")
      if (sp < 0)
        push(params, rest)
        rest = ""
      else
        push(params, substr(rest, 0, sp))
        rest = str_trim(substr(rest, sp + 1, len(rest) - sp - 1))

    return {
      "prefix": prefix,
      "command": str_to_upper(command),
      "params": params,
      "trailing": trailing
    }

  // Convenience: process incoming messages by calling a callback for each line
  // callback(line, msgMap) where msgMap is output of parse_message
  fun pump(this, callback)
    lines = this.read_lines()
    i = 0
    while (i < len(lines))
      ln = lines[i]
      msg = this.parse_message(ln)
      // Auto-reply to PINGs
      if (msg["command"] == "PING")
        t = msg["trailing"]
        if (len(t) == 0 && len(msg["params"]) > 0)
          t = msg["params"][0]
        this.pong(t)
      // Invoke user callback
      if (typeof(callback) == "Function")
        callback(ln, msg)
      i = i + 1
    return len(lines)
