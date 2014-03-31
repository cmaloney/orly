#!/usr/bin/env python2
"""Mynde hello world (Python)

Run stigi in your favorite configuration. For memory simulator you can do:
../out/debug/stig/server/stigi --mem_sim --instance_name=mofo --create=false --starting_state=SOLO -la -le --mem_sim_mb=512 --mem_sim_slow_mb=256

Run this script

Correct output is:
Get Key hello: world

If your output differs (and it will at the moment), it means mynde's memcache implementation is broken.

See the mynde docs for details about what is / isn't implemented at this point in time"""
import pylibmc

#Connect to Mynde
mc = pylibmc.Client(["127.0.0.1"], binary = True, behaviors={"tcp_nodelay": True})

#Set key 'hello' to value 'world'
mc.set("hello", "world")

#Get them back
print "Get Key hello:", mc.get("hello")
