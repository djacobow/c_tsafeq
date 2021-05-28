# c++_tsafeq

simple, c++-based, thread-safe queue using std lib.

## What

A "C++" generic queue that works nicely on any type, and
works properly in a threaded environment, provided 
that `<mutex>` works

## Why?

If you google queues on the Interwebs, I get a bunch that are
just not right or which have an API I don't like.

This one is mine, and I like it fine.

## How

Just include it and instantiate with your type.
