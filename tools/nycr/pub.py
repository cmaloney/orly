#!/usr/bin/env python3
# Copyright 2010-2014 Stig LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#Use this script to make nycr.stable use the parser which was being used by nycr.

import argparse, os, os.path, subprocess

#NOTE: This is a fragile program. Handle with care. It just does a cp with overwrite, so not too horribly dangerous, but
#it is your fault if you mess up.
parser = argparse.ArgumentParser(description='Promote ncyr.dev current state to nycr')
parser.add_argument('-r', '--root', dest='root', help="""The JHM project root (usually ~/projects)""")
parser.add_argument('-c', '--config', dest='config', default='debug', help="""The JHM config that was used (debug, release, etc.)""")
parser.add_argument('-s', '--source_dir', dest='src', default='src', help="""The JHM config that was used""")

args = parser.parse_args();
if not(args.root):
  print("You must specify the JHM root. You're welcome to make it autodetected")
  exit(-1)

j = os.path.join

out_dir = j(args.root, args.src, 'tools/nycr/syntax')
in_dir = j(args.root, 'out', args.config, 'tools/nycr/')
j2 = lambda name: j(in_dir, name);

files = [ 'nycr.bison.h', 'nycr.flex.h', 'nycr.cst.cc', 'nycr.cst.h', 'nycr.l', 'nycr.y', 'nycr.xml', 'nycr.dump.cc' ]

rel_renames = []
#Figure out all the relative renames
for f in files:
  rel_renames.append((j('tools/nycr','nycr.' + f), j('tools/nycr/syntax', f)))

for f in files:
  with open(j2('nycr.' + f),"r") as fin:
    with open(j(out_dir,f),"w") as fout:
        #Copy over contents, updating #include
      for line in fin:
        for in_rel, out_rel in rel_renames:
          line = line.replace(in_rel, out_rel)
        fout.write(line)