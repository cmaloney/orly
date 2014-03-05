This is the repository for the Stig non-relational database. It's meant to be fast and to scale for billions of users.  Stig provides a single path to data and will eliminate our need for memcache due to its speed and high concurrency.

## Stig features:

* **Points of View**: This is our version of optimistic locking or isolation. In traditional databases, clients have to lock the entire database (or at least large swaths of it) before updating it to ensure data remains consistent. In Stig, clients make changes in their own private points of view, which are like small sandboxes. Changes in private points of view eventually propagate into shared points of view and eventually reach the global point of view, which is the whole database. Updates to private points of view don't lock anything: Stig determines later whether, when, and how to reconcile changes from different points of view. We also encourage field calls rather than field changes (e.g., `x += 1` is better than `x = x + 1`).
* **Time Travel**: We use something called the _Flux Capacitor_ to keep a history of changes made to the database and to resolve conflicts as they come into shared points of view and the global point of view. This permits us to perform consistent read for any point in time. Stig defines its "time line" by causality rather than clock time. Instead of manipulating timestamps, Stig records an ordering of events (e.g., update A affects update B, so A "happens before" B).)
* **Query Language**: Stig has its own high-level, compiled, type-safe, functional language called _Stigscript_. Stigscript is not just a query language: You can write general-purpose programs in it complete with compile-time unit tests. Stig comes with a compiler that transforms Stigscript into shared libraries (.so files on Linux), which Stig servers load as packages.
* **Scalability and Availability**: While we eventually plan to develop a sharded Stig machine (and actively design so that we can build such a machine), our current single-node server with fail-over/replication can handle hundreds of thousands of transactions per second. We like to say that Stig will function on a "planetary scale": Your data and computations will not only distribute across a data center, but also across many data centers across the globe. This means that no disaster short of nuking the planet fifty times over or colliding with a gigantic asteroid will destroy your data. (Even those might not be catastrophic: Maybe we'll have data centers running Stig with your replicated data on the moon or Mars.)

## Supported Platforms

Currently Stig only runs on Linux.

## System Requirements

TBD (HDD? SSD? RAM? Processor speed? FS type?)

## Building Stig

Requires the latest versions of the gcc and clang/LLVM compilers. Prior to building Stig, you **must** have applied our patches to the compilers and standard library.

See docs/build.md for precise instructions

## Contributing to Stig

Documentation for this is currently minimal. See docs/coding.md and docs/best_practices.md for basic starting hints.

-----

README.md Copyright 2010-2014 Stig LLC

README.md is licensed under a Creative Commons Attribution-ShareAlike 4.0 International License.

You should have received a copy of the license along with this work. If not, see <http://creativecommons.org/licenses/by-sa/4.0/>.