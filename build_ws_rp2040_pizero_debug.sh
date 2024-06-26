:
# ====================================================================================
# PICO-SMSSPLUS build script in DEBUG configuration
# Builds the emulator for use with the
# Waveshare RP2040-PiZero
# https://www.waveshare.com/rp2040-pizero.htm
# ====================================================================================
cd `dirname $0` || exit 1
# . ./checksdk.sh
if [ -d build ] ; then
	rm -rf build || exit 1
fi
mkdir build || exit 1
cd build || exit 1
cmake -DCMAKE_BUILD_TYPE:STRING=RelWithDebInfo -DSMSPLUS_HW_CONFIG=4 ..
make -j 4
cd ..
# . ./removetmpsdk.sh
