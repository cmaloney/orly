Memcached on Stig
=================

In addition to it's own custom protocol, Stig speaks Memcached to be more easily accessible from arbitrary languages/hosts/etc. Memcached is literally available everywhere at this point.

# Overview

We aim to implement all of the memcached protocol with the same general guarantees that memcached gives, however because we utilize Points of View (TODO: Link) and tetris (TODO: Link) to resolve conflicting updates to the database rather than locks some operations may appear to produce different results (Multiple clients incr the same key won't all get different result values. All their increments will be counted though, and all will get one 'possible' value of the key).

Since Stig is a durable database rather than a cache, we only allow the TTL 0 meaning that keys live forever until deleted. We never cull keys to reclaim space under any circumstance. Once written to the database, things stay in the database until explicitly deleted.

## Durability / Persistence
In general, Stig is a database, not a cache. So items put into it will be persisted. You need to explicitly delete keys for them to go away. TTL must always be specified as '0' which means never expires. Stig will never cull keys under any circumstance. It will stop eventually if it runs out of disk space, but all data will be kept.

## Transactionality
Stig has transactions, meaning it can do both multi-get and multi-set. Exact semantics around this are documented below.

## Consistency
All actions within a single connection happen in a consistent point of view of a database. Specifically, when we recognize a multi-get we guarantee that every key in the multi-get is consistent. Exact rules for this with regards to different database operations appear below.

## Speed
Some operations are going to be considerably faster when running on Stig than they are in memcached because we have less work to do to calculate the result (No locking for most operations). As stig's dataset grows to not fit purely in Memory, some operations may become slower, but we can give strong guarantees on maximum response time given a particular set of hardware.

For writes, generally things will be faster than memcached safe for CAS operations, which will likely be slower.

TODO: Update this section once we have numbers.

## Update Visibility
Stig requires all updates to be put into a private Point of View (TODO: Link to POV Docs) and then promotes the updates to a shared or global point of view.

This maps to memcache in that each connection effectively lives in it's own stig "Session". The session has a single private point of view connected to it. As memcached commands are sent to Stig, the commands are evaluated in that private point of view, and the result (delted, not found, value) is sent back to the user. This all happens without waiting for the result to propogate to a shared point of view.

This is safe to do because the vast majority of memcached operations (basically everything other than cas) don't actually have any preconditions required for them to pass. They just happen or don't. That someone else in parallel could have caused them to happen or not happen is inconsequential. Stig guarantees that the results of every operation returned to the user is theoretically possible. This doesn't mean someone else will ever see that same state, or that when we get around to resolving conflicting updates that will be chosen. As one example, if a delete and an insert happen in parallel, then which occurs first is non-deterministic / a race. Stig will effectively arbitrarily choose when it resolves the difference, but for the initial client calls, both will seem to work.

Every update though will always be seen by the same connection though will see every update which has happened. Over time other connections will see them (Unless the updates get written over, there is no ordering of things that happen simultaneously).

Some operations in Memcached (increment, decrement, prepend, append) modify data in place. We will always return a valid result of the operation given one ordering of time, but exactly what the result is going to end up being when concurrency is resolved may be different. This is no different than someone else just happening to get in before/after you.

# Command Semantics and Guarantees

## Compare and Set (CAS)

CAS operations are the only operations which we actually wait for the promotion to the global point of view to happen before
returning. This is because the semantics defined in the protocol spec require this behavior, and deviating from it modifies the
externally visible results.

## Multi-Get, Multi-Set
These are both transactional in Stig. How we recognize / start / end these varies on which protocol variant you're speaking.

TODO: At present there is no way to do a round of gets and sets together which are guaranteed to be consistent. All the gets will be in one consistent point of view. All the sets will apply to a wholly independent consistent state of the daabase.

### Binary protocol
There is no explicit multi-set or multi-get in the Binary protocol, so we recognize a series of quiet gets followed by anything or a series of quiet sets as one transaction / guaranteed consistent blob. If you want to break a stream of sets or gets into multiple different transactions simply inject an instruction into the stream to break the commands into pieces. A noop or touch (effectively noops since we don't do TTLs) command is recommended for this purpose.

Only quiet gets and quiet sets are watched for being multi-sets/multi-gets. Non-quiet get/set are

### Text Protocol
A get with multiple keys is considered multi-get. A series of independent 'get' commands is considered to be independent transactions.

A series of set commands will be soemantically recognized as a multi-set.

CAS sets are considered independent operations. This could be changed in the future, but for now it's simplest.

## Increment, Decrement
The exact value returned is always a theoretically possible value for the key. It is entirely possible no one else will
ever see it because the value is overwritten, the increment/decrement is combined with others (The operations are NOT
serialized, so multiple people can increment and get the same result).

We don't do any sort of recognition of increment/decrement commands as multi-sets at this juncture. In the future we may
consider a stream of quiet increment/decrement commands. TODO: Revisit this with users to see if they want these to be rolled
into a transaction (Which would be necessary to use this for any sort of credits balance)

## Flush All
The behavior here is TBD. Not currently supported

## Touch
Touch is a no-op since we don't do TTLs