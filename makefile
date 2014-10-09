

all: camera lidar VectorNav

camera:
	cd flycapture.2.6.3.4_armhf/src/CustomImageEx && $(MAKE)

lidar:
	cd urg_library-1.1.3 && $(MAKE)

VectorNav:
	cd vnccpplib-0.3.2/examples/vn200_linux_binary_async && $(MAKE)

