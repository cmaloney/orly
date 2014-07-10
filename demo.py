#!/usr/bin/env python3

# NOTES
#   1. This script should live in demo/ directory
#   2. The bin/ directory must be added to your PATH
#
# SETUP folder hierarchy (Default configuration)
# demo/
#   demo.py   -- demo runner
#   bin/      -- binaries
#   data/     -- datasets in their own directories.
#   packages/ -- pre-supplied packages for the datasets
#   webui/    -- checkout of git@github.com:orlyatomics/orly-webui.git
#
# Then run ./demo.py {dataset}
#
# If you have local configurations, you can specify them via the command line
# arguments. For example, I have a 'demo.local' file with contents:
#
#   --webui-dir=/home/mpark/OrlyAtomics/webui
#   --mem-sim-mb=512
#
# Then run ./demo.py {dataset} `< demo.local`

import argparse
import os.path
import signal
import subprocess
import threading
import time

def print_orlyi(stdout):
    while True:
        print(stdout.readline().decode('UTF-8').strip())

def ctrl_c(signal, frame):
    exit()

assert __name__ == '__main__'
cwd = os.getcwd()
datasets = ['enron', 'matrix', 'social_graph', 'twitter']
# Argparse.
parser = argparse.ArgumentParser(description='Run the Orly demo.')
parser.add_argument(
    'dataset', choices=datasets, help='dataset to load.', type=str)
parser.add_argument(
    '-d', '--data-dir',
    default=os.path.join(cwd, 'data'),
    help='directory where data lives.')
parser.add_argument(
    '-p', '--packages-dir',
    default=os.path.join(cwd, 'packages'),
    help='directory where packages live.')
parser.add_argument(
    '-w', '--webui-dir',
    default=os.path.join(cwd, 'webui'),
    help='directory where the webui live.')
parser.add_argument(
    '-m', '--mem-sim-mb',
    default=4096, help='amount of memory to be used by orlyi.', type=int)
args = parser.parse_args()
# Check for validity of directories.
dataset_dir = os.path.join(args.data_dir, args.dataset)
for path in [args.data_dir, args.packages_dir, args.webui_dir, dataset_dir]:
    if not os.path.isdir(path):
        raise RuntimeError(
                  '{} must be an existing directory.'.format(path))
# Launch orlyi.
try:
    orlyi = subprocess.Popen(
                ['orlyi',
                    '--create=true',
                    '--block_cache_size={}'.format(args.mem_sim_mb / 4),
                    '--instance_name={}'.format(args.dataset),
                    '--la',
                    '--le',
                    '--mem_sim=true',
                    '--mem_sim_mb={}'.format(args.mem_sim_mb),
                    '--package_dir={}'.format(args.packages_dir),
                    '--page_cache_size={}'.format(args.mem_sim_mb / 4),
                    '--starting_state=SOLO',
                    '--update_pool_size=250002',
                    '--update_entry_pool_size=1000008'],
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT)
    laravel = subprocess.Popen(
                  ['php', os.path.join(args.webui_dir, 'artisan'), 'serve'],
                  stderr=subprocess.STDOUT)
    # Register signal handler for Ctrl-C.
    signal.signal(signal.SIGINT, ctrl_c)
    # Read orlyi output until it outputs TServer::CleanHouse().
    while True:
        line = orlyi.stdout.readline().decode('UTF-8').strip()
        print(line)
        if line == 'TServer::CleanHouse()':
            break
    # Launch a daemon thread which pipes the output from orlyi to stdout.
    thread = threading.Thread(target=print_orlyi, args=(orlyi.stdout,))
    thread.daemon = True
    thread.start()
    # Import data
    for root, _, files in os.walk(dataset_dir):
        if files:
            subprocess.check_output(
                ['core_import',
                 '--la',
                 '--le',
                 '--import_pattern={}'.format(os.path.join(root, '*.bin*')),
                 '--num_load_threads=1',
                 '--num_merge_threads=1',
                 '--num_sim_merge={}'.format(max(8, len(files)))],
                stderr=subprocess.STDOUT)
    line = '{:=^80}'.format('')
    msg = lambda msg: '{0:=^5}{1: ^70}{0:=^5}'.format('', msg)
    print(line)
    print(line)
    print(msg('Orly ready to demo'))
    print(msg('Go to http://localhost:8000/console'))
    print(msg('Press Ctrl-C to shutdown'))
    print(line)
    print(line)
    # Wait for Ctrl-C
    signal.pause()
finally:
    print('Shutting down')
    orlyi.kill()
    laravel.send_signal(signal.SIGINT)
