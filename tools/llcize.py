#!/usr/bin/python2
#
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


from __future__ import print_function

import argparse, os, re, sys


OLD_COPYRIGHT_PATTERN = re.compile(R'copyright[ \t]*\d+(\-\d*)?[ \t]*tagged', re.I)

NEW_COPYRIGHT = 'Copyright 2010-2014 Stig LLC'

Root = ''
IgnoredRelPaths = []
IgnoredExts = []


def main():
  def ParseArgs():
    global Root, IgnoredRelPaths, IgnoredExts
    parser = argparse.ArgumentParser(description='Update Stig source files to include Apache license.')
    parser.add_argument('root', help='The root of the tree to scan for source files.')
    parser.add_argument('--ignore_rel_path', help='A relative path to ignore while scanning for source files.', action='append')
    parser.add_argument('--ignore_ext', help='A file extension to ignore while scanning for source files.', action='append')
    args = parser.parse_args()
    Root = os.path.realpath(args.root)
    IgnoredRelPaths = args.ignore_rel_path or []
    IgnoredRelPaths.append('.git')
    IgnoredRelPaths = frozenset(IgnoredRelPaths)
    IgnoredExts = args.ignore_ext or []
    IgnoredExts = frozenset(IgnoredExts)
  ParseArgs()
  root_prefix_len = len(Root) + 1
  for abs_path, dir_names, file_names in os.walk(Root):
    # Strip the root prefix from the abs path to make the rel path.
    rel_path = abs_path[root_prefix_len:]
    # Drop dir names that should be ignored.
    filtered_dir_names = []
    for dir_name in dir_names:
      if os.path.join(rel_path, dir_name) not in IgnoredRelPaths:
        filtered_dir_names.append(dir_name)
    dir_names[:] = filtered_dir_names
    # Scan files.
    for file_name in file_names:
      # Strip any '.hold' ext.
      base = file_name
      while True:
        base, ext = os.path.splitext(base)
        if ext != '.hold':
          break
      # If the ext should be ignored, skip the file.
      if ext in IgnoredExts:
        continue
      ex = None
      is_handled = False
      try:
        # Read the text of the file into memory.
        file_rel_path = os.path.join(rel_path, file_name)
        file_abs_path = os.path.join(Root, file_rel_path)
        text = open(file_abs_path, 'r').read()
        # If the file is empty, skip the file.
        if not text:
          continue
        # If the text of the file doesn't match the old copyright pattern, skip the file.
        match = OLD_COPYRIGHT_PATTERN.search(text)
        if not match:
          continue
        # Replace the copyright notice
        text = '%s%s%s' % (text[:match.start()], NEW_COPYRIGHT, text[match.end():])
        # Strip trailing spaces
        text = '\n'.join([line.rstrip() for line in text.splitlines()])
        # Replace the file with the transformed text.
        temp_abs_path = file_abs_path + '.safe'
        open(temp_abs_path, 'w').write(text)
        os.rename(temp_abs_path, file_abs_path)  # This *should* be atomic on Linux.  Right... ?
        is_handled = True
      except Exception, ex:
        pass
      # Report our results.
      print(
        '%s %r%s' % ('fixed' if is_handled else 'check', file_abs_path, ' %r' % ex if ex else ''),
        file=sys.stdout if is_handled else sys.stderr)


if __name__ == '__main__':
  exit(main())
