CONFIG_MODULE_SIG=n
obj-m = vcam.o
vcam-objs = device_ops.o v4l2_ops.o v4l2_timer.o v4l2_ioctl.o buffer_queue_ops.o myvcam.o
TEST_TARGET := V4L2_app_test
APP := test

.PHONY: all
all: kmod

kmod:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc -o $(APP) $(TEST_TARGET).c

.PHONY: clean
clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm $(APP)