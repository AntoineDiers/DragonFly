SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

cd $SCRIPT_DIR

rm -rf build
mkdir build
cd build
cmake ..
make

cd ..
rm -rf ./include/dragonfly_msgs/msg
mkdir ./include/dragonfly_msgs/msg
./build/dragonfly_msgs_generator .