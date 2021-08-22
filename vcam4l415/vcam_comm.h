#ifndef __VCAM_COMM_H__
#define __VCAM_COMM_H__
#include <media/videobuf-vmalloc.h>
#include <media/v4l2-device.h>
#include <media/v4l2-ioctl.h>


struct vcam_data {
    struct videobuf_queue vqueue;
    struct list_head vb_local_queue;
    struct v4l2_format format;
};

struct myvcam_device {
    struct video_device *vdev;
    struct v4l2_device v4l2_dev; // 待第二版更新，本次不更新
    spinlock_t queue_slock;
    struct timer_list timer;
    struct vcam_data vdata;
    void *private_data;
};

extern struct myvcam_device *vcam_dev;

void myvivi_precalculate_bars(int input);
void myvivi_timer_func(struct timer_list *list);
void vcam_release(struct video_device *vdev);

#endif

