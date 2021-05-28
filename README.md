# c_tsafeq

simple, c-base, fixed-allocation thread-safe queue

## What

A "C" generic queue that works nicely on any type, does
not call allocators (fixed allocation) and works properly
in a threaded environment, provided that pthread works.

## Why?

If you google queues on the Interwebs, I get a bunch that are
just not right or which have an API I don't like.

This one is mine, and I like it fine.

## How

There is a small example program.

Building is trivial. Typically, you should only neead -lpthread

