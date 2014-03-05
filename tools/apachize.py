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


COPYRIGHT_PATTERN = re.compile(R'copyright[ \t]*\d+(\-\d*)?[ \t]*tagged', re.I)

APACHE_PATTERN = re.compile(R'Licensed under the Apache License')

APACHE_NOTICE_LINES = [
  'Copyright 2010-2014 Stig LLC',
  '',
  'Licensed under the Apache License, Version 2.0 (the "License");',
  'you may not use this file except in compliance with the License.',
  'You may obtain a copy of the License at',
  '',
  '  http://www.apache.org/licenses/LICENSE-2.0',
  '',
  'Unless required by applicable law or agreed to in writing, software',
  'distributed under the License is distributed on an "AS IS" BASIS,',
  'WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.',
  'See the License for the specific language governing permissions and',
  'limitations under the License.' ]


Root = ''
IgnoredRelPaths = []
IgnoredExts = []


def CHandler(rel_path, text):
  if not text.startswith('/*'):
    return None
  star_slash = text.find('*/')
  if star_slash < 0:
    return None
  lines = text[:star_slash].splitlines()
  if not COPYRIGHT_PATTERN.search(lines[-1]):
    return None
  del lines[-1]
  if lines:
    del lines[0]
  else:
    lines = [ '' ]
  lines.insert(0, '/* <%s> ' % rel_path)
  lines.extend([ '   ' + line for line in APACHE_NOTICE_LINES ])
  return '%s %s' % ('\n'.join(lines), text[star_slash:])


def ScriptHandler(rel_path, text):
  lines = text.splitlines()
  if not lines[0].startswith('#!'):
    return None
  found = False
  for idx in xrange(1, len(lines)):
    if not lines[idx].startswith('#'):
      break
    if COPYRIGHT_PATTERN.search(lines[idx]):
      lines[idx:idx + 1] = [ '# ' + line for line in APACHE_NOTICE_LINES ]
      found = True
      break
  return '\n'.join(lines) if found else None


def NoHandler(rel_path, text): return None


HANDLERS = {
  '.cc':   CHandler,
  '.h':    CHandler,
  '.stig': CHandler,
  '.nycr': CHandler,
  '.sh':   ScriptHandler,
  '.py':   ScriptHandler,
}


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
        # If the text of the file doesn't match our copyright pattern, skip the file.
        if not COPYRIGHT_PATTERN.search(text):
          continue
        # If the text already matches the Apache pattern, skip the file.
        if APACHE_PATTERN.search(text):
          continue
        # If we can transform the text, write it back out.
        handler = HANDLERS.get(ext)
        text = handler(file_rel_path, text) if handler else None
        if text is not None:
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
