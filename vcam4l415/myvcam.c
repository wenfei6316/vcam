#include <linux/module.h>
#include "vcam_comm.h"
#include "v4l2_ops.h"

struct myvcam_device *vcam_dev = NULL;
EXPORT_SYMBOL(vcam_dev);


static const struct v4l2_file_operations v4l2_fops = {
    .owner = THIS_MODULE,
    .open = v4l2_open,
    .unlocked_ioctl = video_ioctl2, /* V4L2 ioctl handler */
    .mmap = v4l2_mmap,
    .poll = v4l2_poll,
    .release = v4l2_close,
};

static const struct v4l2_ioctl_ops v4l2_ioctl_ops = {
    // 表示他是一个摄像头设备
    .vidioc_querycap = myvivi_vidioc_querycap,

    // 用于列举、获得、测试、设置摄像头的数据的格式
    .vidioc_enum_fmt_vid_cap = myvivi_vidioc_enum_fmt_vid_cap, // OK
    .vidioc_g_fmt_vid_cap = myvivi_vidioc_g_fmt_vid_cap, // OK
    .vidioc_try_fmt_vid_cap = myvivi_vidioc_try_fmt_vid_cap, // OK
    .vidioc_s_fmt_vid_cap = myvivi_vidioc_s_fmt_vid_cap, // OK
    // 缓冲区操作：申请、查询、放入队列、取出队列
    .vidioc_reqbufs = myvivi_vidioc_reqbufs,
    .vidioc_querybuf = myvivi_vidioc_querybuf,
    .vidioc_qbuf = myvivi_vidioc_qbuf,
    .vidioc_dqbuf = myvivi_vidioc_dqbuf,
    // 启动、停止
    .vidioc_streamon = myvivi_vidioc_streamon,
    .vidioc_streamoff = myvivi_vidioc_streamoff,
};

static int myvcam_device_init(struct myvcam_device *vcam_dev)
{
    int ret = 0;
    if (!vcam_dev) {
        pr_err("vcam_dev is NULL\n");
        return -ENODEV;
    }

    snprintf(vcam_dev->v4l2_dev.name, sizeof(vcam_dev->v4l2_dev.name), "myvivi");
    ret = v4l2_device_register(NULL, &vcam_dev->v4l2_dev);
    if (ret) {
        pr_err("v4l2 registration failure\n");
        goto free_dev;
    }

    // 申请video_device结构体并初始化
    vcam_dev->vdev = video_device_alloc();
    if (!vcam_dev->vdev) {
        pr_err("alloc video_device error\n");
        ret = -ENOMEM;
        goto free_dev;
    }

/*************************************************************************************/
    // 新架构需要操作v4l2_dev的结构体，待操作
    // Todo
/*************************************************************************************/

    vcam_dev->vdev->release = vcam_release;
    vcam_dev->vdev->fops = &v4l2_fops;
    vcam_dev->vdev->ioctl_ops = &v4l2_ioctl_ops;
    vcam_dev->vdev->v4l2_dev = &vcam_dev->v4l2_dev;
    snprintf(vcam_dev->vdev->name, sizeof(vcam_dev->vdev->name), "%s", vcam_dev->v4l2_dev.name);
    // 2.4 队列操作
    // a.定义、初始化一个队列（会用到一个spinlock）
    spin_lock_init(&vcam_dev->queue_slock);
    INIT_LIST_HEAD(&vcam_dev->vdata.vb_local_queue);
    // 初始化定时器
    vcam_dev->timer.function = myvivi_timer_func;
    vcam_dev->timer.expires = jiffies + 1;
    //init_timer(&vcam_dev->timer);
    timer_setup(&vcam_dev->timer, myvivi_timer_func, 0);
    return 0;
free_dev:
    kfree(vcam_dev);
    return ret;
}

static int __init myvcam_init(void)
{
    int ret;
    pr_info("Enter: %s [line: %d]\n", __func__, __LINE__);
    // 1. 申请设备结构体
    vcam_dev = kzalloc(sizeof(struct myvcam_device), GFP_KERNEL);
    if (!vcam_dev) {
        pr_err("kzalloc vcam_dev error\n");
        return -ENOMEM;
    }
    // 2. 初始化设备结构体
    ret = myvcam_device_init(vcam_dev);
    if (ret) {
        pr_err("device init error\n");
        goto init_dev;
    }

    // 3. 注册设备
    ret = video_register_device(vcam_dev->vdev, VFL_TYPE_GRABBER, -1);
    if (ret) {
        pr_err("device init error\n");
        goto init_dev;
    }

init_dev:
    return ret;
}

static void __exit myvcam_exit(void)
{
    pr_info("Enter: %s [line: %d]\n", __func__, __LINE__);
    if (vcam_dev && vcam_dev->vdev) {
        video_unregister_device(vcam_dev->vdev);
        video_device_release(vcam_dev->vdev);
        kfree(vcam_dev);
    }
}


module_init(myvcam_init);
module_exit(myvcam_exit);
MODULE_LICENSE("GPL");

