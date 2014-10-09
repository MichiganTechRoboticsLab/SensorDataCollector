TARGET = sensor_parameter get_distance get_distance_intensity get_multiecho get_multiecho_intensity sync_time_stamp # LIDAR_COMP LIDAR_OMG LIDAR_OMG_matlab calculate_xy find_port

URG_LIB = ../../src/liburg_c.a

include ../../build_rule.mk

#gcc -ggdb `pkg-config --cflags opencv` -o `basename opencvtest.c .c` opencvtest.c `pkg-config --libs opencv`

CFLAGS = -Wall -ggdb -O0 $(INCLUDES) -I../../include/c -I/usr/local/include -L/usr/local/cuda/lib
LDLIBS = -lm `/bin/sh ld_wsock.sh` `/bin/sh ld_setupapi.sh` # `pkg-config --cflags opencv` `pkg-config --libs opencv` -L/usr/local/lib -lgsl -lgslcblas -lm

all : $(TARGET)

clean :
	$(RM) *.o $(TARGET) *.exe

$(TARGET) : open_urg_sensor.o $(URG_LIB)

$(URG_LIB) :
	cd $(@D)/ && $(MAKE) $(@F)
