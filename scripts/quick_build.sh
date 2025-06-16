#!/bin/bash
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

set -e
cd $SCRIPT_DIR/.. # Change to project root
cmake -B build --preset default
cmake --build build