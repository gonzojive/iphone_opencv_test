
OPENCV="OpenCV-2.1.0"

all: opencv

clean:
	if test -d $(OPENCV); \
	then rm -rf $(OPENCV); \
	fi

extract-opencv:
	if test -d $(OPENCV); \
	then echo $(OPENCV) exists, no need to extract; \
	else tar xjf $(OPENCV).tar.bz2; \
	fi

patch-opencv: extract-opencv
	cd $(OPENCV); patch -p1 < ../$(OPENCV).patch

opencv-simulator: patch-opencv
	mkdir -p build_simulator; \
	cd build_simulator; \
	../opencv_cmake.sh Simulator ../$(OPENCV); \
	make -j 4; \
	make install;

opencv-device: patch-opencv
	mkdir -p build_device; \
	cd build_device; \
	../opencv_cmake.sh Device ../$(OPENCV); \
	make -j 4; \
	make install;

opencv: opencv-simulator opencv-device

