#!/bin/bash

#NOTE: This script should live in the user's home folder
#      All binaries for orly should be in PATH (Add the bin/ directory to PATH)
#
# SETUP folder hierarchy
# orly/
#   web/  -- Checkout of orly-web
#   src/  -- Checkout of orly (NOTE: Must be the same path used when compiling orlyi)
#   bin/  -- All the binaries (This gets added to the user's path)
#   data/   -- All the .bin files which can be loaded
#
# Then run ./demo.sh {dataset}

usage() {
  echo "Usage: $0 [DATASET]"
  echo "  DATASET = {beer|complete_graph|game_of_thrones|money_laundering|belgian_beer|friends_of_friends|matrix|shakespeare|twitter}"
  exit 1
}

DATASET=$1

if [ $# -ne 1 ]; then usage; fi

DATA_FILE="$DATASET.bin"
case $DATASET in
  "beer") ;;
  "complete_graph") ;;
  "game_of_thrones") ;;
  "money_laundering") ;;
  "belgian_beer") ;;
  "friends_of_friends") ;;
  "matrix") ;;
  "shakespeare") ;;
  "twitter") DATA_FILE="twitter0[0-6].bin" ;;
  *) usage
esac

ROOT="$PWD"
DATA_DIR="$ROOT/orly/data"
PACKAGE_DIR="$ROOT/orly/packages"
SRC_DIR="$ROOT/orly/src"
WEB_DIR="$ROOT/orly/web"

#TODO: Download all the bin files we want if twitter. Otherwise run the generator utility, to make the .bin file.
FILENAME="$DATA_DIR/$DATA_FILE"
NUM_FILES=`ls -1 $FILENAME | wc -l`
if [ -z $NUM_FILES ]; then
  echo "Requested dataset not ready for importing."
  echo "Run the importer for the dataset and place the resulting bin files as $FILENAME"
  exit -1
fi

#Start orlyi
#TODO: Persistent state files / hard drive
orlyi --create=true --instance_name=$DATASET --starting_state=SOLO --la --le                                           \
  --package_dir="$PACKAGE_DIR"                                                                                         \
  --temp_file_consol_thresh=4                                                                                          \
  --mem_sim=true --mem_sim_mb=768  --page_cache_size=256 --block_cache_size=256                                        \
  --update_pool_size=55000 --update_entry_pool_size=275000 &
ORLY_PID=$!
#TODO : Make a better way to test for the server being up rather than just assuming it starts up in 5 seconds
sleep 5

core_import --num_load_threads=1 --num_merge_threads=1 --la --le                                                       \
  --num_sim_merge="$NUM_FILES" --import_pattern="$FILENAME"
CORE_IMPORT_RETURN=$?

if [ $CORE_IMPORT_RETURN -ne 0 ]; then
  echo "ERROR: Failed to import file $FILENAME"
  kill -9 $ORLY_PID
  exit 0
fi

##TODO: Run orlyc on sample queries to compile

php $WEB_DIR/artisan serve --host &
WEB_PID=$!

echo "==========================================================="
echo "==========================================================="
echo "Orly ready to demo"
echo "Go to http://localhost:8000/console"
echo "Press Ctrl-C to shutdown"
echo "==========================================================="
echo "==========================================================="

control_c()
{
  kill -9 $ORLY_PID
  kill $WEB_PID
  exit 0
}

trap control_c SIGINT
while true; do read x; done