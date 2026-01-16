#!/usr/bin/env bash

# This file is part of the Fun programming language.
# https://fun-lang.xyz/
#
# Copyright 2025 Johannes Findeisen <you@hanez.org>
# Licensed under the terms of the Apache-2.0 license.
# https://opensource.org/license/apache-2-0
#
# Added: 2025-12-12

if [ -z "$1" ]; then
  echo "Build target is unset, using 'minimal'";
  target="minimal";
else
  echo "Build target is set to '$1'";
  target=$1;
fi

if [ "$target" = "all" ]; then
  rm -rf build \
    && cmake -S . -B build \
      -DFUN_WITH_PCSC=ON \
      -DFUN_WITH_REPL=ON \
      -DFUN_WITH_LIBSQL=ON \
      -DFUN_WITH_SQLITE=ON \
      -DFUN_WITH_CURL=ON \
      -DFUN_WITH_PCRE2=ON \
      -DFUN_WITH_XML2=ON \
      -DFUN_WITH_JSON=ON \
      -DFUN_WITH_TCLTK=ON \
      -DFUN_WITH_INI=ON \
      -DFUN_WITH_NOTCURSES=ON \
    && cmake --build build --target fun
elif [ "$target" = "all_debug" ]; then
  rm -rf build \
    && cmake -S . -B build \
      -DFUN_WITH_PCSC=ON \
      -DFUN_WITH_REPL=ON \
      -DFUN_WITH_LIBSQL=ON \
      -DFUN_WITH_SQLITE=ON \
      -DFUN_WITH_CURL=ON \
      -DFUN_WITH_PCRE2=ON \
      -DFUN_WITH_XML2=ON \
      -DFUN_WITH_JSON=ON \
      -DFUN_WITH_TCLTK=ON \
      -DFUN_WITH_INI=ON \
      -DFUN_WITH_NOTCURSES=ON \
      -DFUN_DEBUG=ON \
    && cmake --build build --target fun
elif [ "$target" = "alpine" ]; then
  rm -rf build \
    && cmake -S . -B build \
      -DFUN_WITH_REPL=ON \
      -DFUN_WITH_LIBSQL=ON \
      -DFUN_WITH_SQLITE=ON \
      -DFUN_WITH_CURL=ON \
      -DFUN_WITH_PCRE2=ON \
      -DFUN_WITH_XML2=ON \
      -DFUN_WITH_JSON=ON \
      -DFUN_WITH_INI=ON \
    && cmake --build build --target fun
elif [ "$target" = "debug" ]; then
  rm -rf build \
    && cmake -S . -B build \
      -DFUN_WITH_REPL=ON \
      -DFUN_DEBUG=ON \
    && cmake --build build --target fun
elif [ "$target" = "freebsd" ]; then
  rm -rf build \
    && cmake -S . -B build \
      -DFUN_WITH_REPL=ON \
    && cmake --build build --target fun
elif [ "$target" = "install" ]; then
  sudo cmake --build build --target install
elif [ "$target" = "minimal" ]; then
  rm -rf build \
    && cmake -S . -B build \
    && cmake --build build --target fun
elif [ "$target" = "musl" ]; then
  rm -rf build \
    && cmake -S . -B build \
      -DFUN_WITH_REPL=ON \
      -DFUN_USE_MUSL=ON \
    && cmake --build build --target fun
elif [ "$target" = "repl" ]; then
  rm -rf build \
    && cmake -S . -B build \
      -DFUN_WITH_REPL=ON \
    && cmake --build build --target fun
else
  echo "Build target $target not found... aborting!";
  echo "Available targets:";
  echo " - all";
  echo " - all_debug";
  echo " - alpine";
  echo " - debug";
  echo " - freebsd";
  echo " - install";
  echo " - minimal";
  echo " - musl";
  echo " - repl";
fi

