#!/bin/bash

# NOTE: This script should live in the user's home folder
#       All binaries for orly should be in PATH (Add the bin/ directory to PATH)
#
# SETUP folder hierarchy
# orly/
#   webui/    -- Checkout of orly-webui
#   src/      -- Checkout of orly (NOTE: Must be the same path used when compiling orlyi)
#   bin/      -- All the binaries (This gets added to the user's path)
#   data/     -- All the .bin files which can be loaded
#   packages/ -- Pre-supplied packages for datasets
#
# Then run ./demo.sh {dataset}

usage() {
  echo "Usage: $0 [DATASET]"
  echo "  DATASET = {beer|complete_graph|game_of_thrones|money_laundering|belgian_beer|friends_of_friends|matrix|shakespeare|twitter|twitter_ego}"
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
  "twitter") DATA_FILE="twitter0[0-6].bin.gz" ;;
  "twitter_ego") DATA_FILE="twitter-ego[0-9].bin" ;;
  *) usage
esac

ROOT="$PWD"
DATA_DIR="$ROOT/data"
PACKAGE_DIR="$ROOT/packages"
SRC_DIR="$ROOT/src"
WEB_DIR="$ROOT/webui"

set -e

#Get the datasets
echo "Getting Dataset"
FILENAME="$DATA_DIR/$DATA_FILE"
NUM_FILES=1
if [ "$DATASET" = "twitter" ]; then
  for twitter_datafile in "twitter0"{0..6}".bin.gz"
  do
    TWITTER_FILENAME="$DATA_DIR/$twitter_datafile"
    echo $TWITTER_FILENAME
    if [ ! -f $TWITTER_FILENAME ]; then
      wget -O"$TWITTER_FILENAME" "http://vagrantcloud.orlyatomics.com/box/virtualbox/data/$twitter_datafile"
    fi
  done
  NUM_FILES=7
elif [ "$DATASET" = "twitter_ego" ]; then
  # TODO: load twitter-ego[0-9].bin from somewhere.
  NUM_FILES=10
elif [ ! -f "$FILENAME" ]; then
  #Non-twitter datasets run the generator, then move the bin file
  "$DATASET"
  mv "$DATASET.bin" "$FILENAME"
fi

#Grab the web UI
if [ ! -d "webui" ]; then
  echo "You must download / setup the Web UI before you can run the demo"
fi


#Start orlyi
#TODO: Persistent state files / hard drive
orlyi --create=true --instance_name=$DATASET --starting_state=SOLO --la --le \
  --package_dir="$PACKAGE_DIR" \
  --temp_file_consol_thresh=4 \
  --mem_sim=true --mem_sim_mb=2048  --page_cache_size=512 --block_cache_size=512 \
  --update_pool_size=350000 --update_entry_pool_size=350000 &
ORLY_PID=$!
#TODO : Make a better way to test for the server being up rather than just assuming it starts up in 15 seconds
sleep 15

set +e


core_import --num_load_threads=1 --num_merge_threads=1 --la --le \
  --num_sim_merge="$NUM_FILES" --import_pattern="$FILENAME"
CORE_IMPORT_RETURN=$?

if [ $CORE_IMPORT_RETURN -ne 0 ]; then
  echo "ERROR: Failed to import file $FILENAME"
  kill -9 $ORLY_PID
  exit 0
fi

##TODO: Run orlyc on sample queries to compile

php -S 0.0.0.0:8000 -t webui/public webui/server.php &
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
  kill -s INT $WEB_PID
  exit 0
}

trap control_c SIGINT
while true; do read x; done
