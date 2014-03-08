#!/usr/bin/env python2
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

import argparse
import cPickle
import itertools
import multiprocessing
import os.path
import subprocess

from littleworkers import Pool

# Process object. wraps subprocess.Popen object to augment it with a filename.
class TProcess(object):
  def __init__(self, filepath, process):
    self.filepath = filepath
    self.process = process
    self.pid = process.pid
    self.stdout = process.stdout

  def poll(self):
    return self.process.poll()

  def returncode(self):
    return self.process.returncode

# Our Process pool inherited from littleworkers' Pool
class TPool(Pool):
    def __init__(self, out_dir, *args, **kwargs):
        super(TPool, self).__init__(*args, **kwargs)
        self.collected_output = []
        self.__out_dir = out_dir

    def create_process(self, filepath):
        # logging.debug("Starting process to handle command '%s'." % command)
        return TProcess(
                 filepath,
                 subprocess.Popen(
                     '../../../out/debug/stig/stig -m -d ' + filepath + ' -o ' + self.__out_dir,
                     shell=True,
                     stderr=subprocess.STDOUT,
                     stdout=subprocess.PIPE))

def GetArgParser():
  default_worker_count = multiprocessing.cpu_count()

  # Parse command line arguments
  parser = argparse.ArgumentParser(description='Report changes in tests')
  parser.add_argument('filepaths', type=str, nargs='*',
      help='The list of stigscript files to use, defaults to all *.stig files in the current directory')
  parser.add_argument('--update', '-u', action='store_true', help='Update the state files for the given stigscript files')
  parser.add_argument('--changes', '-c', action='store_true', help='Print out the changes')
  parser.add_argument(
      '--worker-count', '-w',
      default=default_worker_count,
      type=int,
      help='Specify the number of workers for the script. (default: ' + str(default_worker_count) + ')')
  return parser

def GetResult(proc):
  output = proc.stdout.read()
  sections = map(lambda x: x.splitlines(), output.split('MM_NOTICE: '))
  return proc.returncode(), sections

def GetStateFilename(filepath):
  dirpath, filename = os.path.split(filepath)
  return os.path.join(dirpath, '.' + filename + '.test.state')

def Main():
  args = GetArgParser().parse_args()

  # We should use mkdtemp, but that creates a new directory name for every build, giving us false positives for output changing.
  # out_dir = tempfile.mkdtemp(prefix = "stig") # Temp Directory
  out_dir = '/tmp/stig_compiler/'
  try:
    os.mkdir(out_dir)
  except OSError:
    pass

  # Singleton instance of our process pool
  lil = TPool(out_dir, workers=args.worker_count)


  def OnDirectory(none, dirname, filenames):
    stig_filenames = filter(lambda filename: os.path.splitext(filename)[-1] == '.stig', filenames)
    filepaths.extend(map(lambda filename: os.path.join(dirname, filename), stig_filenames))

  # List of filepaths.
  filepaths = args.filepaths

  if len(filepaths) == 0:
    cwd = os.getcwd()
    prefix_len = len(cwd) + 1
    os.path.walk(os.getcwd(), OnDirectory, None)
    filepaths = map(lambda x: x[prefix_len:] if x.startswith(cwd) else x, filepaths)

  changed_files = []
  passed_files = []
  failed_files = []

  def Callback(proc):
    # Get the result
    result = GetResult(proc)
    filepath = proc.filepath
    if args.update:
      cPickle.dump(result, open(GetStateFilename(filepath), 'w'))
      return
    else:
      try:
        expected = cPickle.load(open(GetStateFilename(filepath), 'r'))
      except IOError as ex:
        if ex.errno != 2:
          raise
        if result[0] != 0:
          print 'No state file:', filepath
          changed_files.append(filepath)
          failed_files.append(filepath)
        else:
          passed_files.append(filepath)
        return

      # Did we pass?
      passed = result[0] == 0

      if result == expected:
        if passed:
          passed_files.append(filepath)
        else:
          failed_files.append(filepath)
        return

      returncode, section = result
      expected_returncode, expected_section = expected

      print '===================================================================='
      print 'Changes:', filepath

      acceptable_change = True

      for (expected_val, val) in itertools.izip_longest(expected_section, section):
        if expected_val == val:
          continue
        elif expected_val is None:
          acceptable_change = False
          print 'Further:', val[0]
          if args.changes:
            print '\n'.join(val)
        elif val is None:
          acceptable_change = False
          print 'Worse:', expected_val[0]
        else:
          print 'Differs:', expected_val[0]
          if args.changes:
            print 'Old:'
            if len(expected_val) > 0:
              print '\n'.join(expected_val[1:])
            print 'New:'
            if len(val) > 0:
              print '\n'.join(val[1:])

      if returncode != expected_returncode:
        acceptable_change = False
        print 'Return code from', expected_returncode, 'to', returncode

      if not acceptable_change:
        changed_files.append(filepath)

      if passed:
        passed_files.append(filepath)
      else:
        failed_files.append(filepath)
      return

  lil.run(filepaths, callback=Callback)

  if not args.update:
    print 'Change:', ','.join(changed_files)
    print 'Pass:', ','.join(passed_files)
    print 'Fail:', ','.join(failed_files)
    print 'Overall: %s changed, %s passed, %s failed' % (len(changed_files), len(passed_files), len(failed_files))

if __name__ == '__main__':
  Main()
