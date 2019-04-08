file info have kind-specific

jobs are generators
if a job needs to split, it says "needs these results" + either a "partial results useful" vs. "need all" (one new header available worth doing, one new .o which won't change link needs doesn't)


Question: What happens if you change file info to be type specific?

so a "C++ File" 
has a "as an executable"
and a "get my dependencies"
and a "turn into a object file"

Can then do a .As<>/.TryAs<> on a file to convert it to the given type.

A job can than say "Make real" / "make this happen". That is "generate the done file"

There's a helper to say "make many happen, where it will take one which it will continue
on, and the rest will be "forked" coroutines. When the core job gets back 



so basically:
 - Can always get a FileInfo for any given type + path, no co-ordination to get (just effectively rel_path manipulation)
   - Can maybe query "feasiblity" (although that really wants a cache to avoid massive recomputation...)
 - Can do a .As<TTargetFileType> which returns a task which generates the target type
   - Can do a .TryAs<> to find out "is this feasible to build"?
 - Can co_await the task to get its final info object (will talk into the global ready store)
   - Optimistically finish if the task is marked as done (cached results)
   - Ideally can also cache results to disk for not recently used ones (think two stage builds)
 - Can ask the specific task for arbitrary type-specific details. (ex. dependencies)
 - The "task" wrapper probably contains all the "pull type-specific details out of cache file and rebuild.
 - Have a "runtime" task type system for dynamically loaded things (adding new types, new external build commands / helpers). Use JSON as standard interchange format (can add more efficient encoding later if number of bytes becomes a problem)
 -

side tasks:
 - corotuines for base/pump (and run arbitrary thing in general)


This basically means
 - The "needs" system is no more

Plan of attack
 - Rewrite the basic compile jobs into new structure
 - Start with a single thread of execution / no "splitting" or dynamic picking back up. Just use task<>
 - 