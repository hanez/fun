#!/usr/bin/env fun

#include <net/cgi.fun>

cgi = CGI()
pairs = cgi._parse_urlencoded("a=1&b=2&c=3")
i = 0
n = len(pairs)
while (i < n)
  p = pairs[i]
  print(to_string(p[0]) + "=" + to_string(p[1]))
  i = i + 1
