#include "buffer_queue_ops.h"

/*
 * App调用ioctl VIDIOC_REQBUFS时会导致此函数被调用，
 * 它重新调整count和size
 */

int myvivi_buffer_setup(struct videobuf_queue *q, unsigned int *count, unsigned int *size)
{
    pr_info("Enter: %s [line: %d]\n", __func__, __LINE__);

    *size = vcam_dev->vdata.format.fmt.pix.sizeimage;
    if (0 == *count)
        *count = 32;
    return 0;
}

/*
 * App调用ioctl VIDIOC_QBUF时导致此函数被调用
 * 它会填充video_buffer结构体并调用videobuf_iolock来分配内存
 */
int myvivi_buffer_prepare(struct videobuf_queue *vq, struct videobuf_buffer *vb, enum v4l2_field field)
{
    pr_info("Enter: %s [line: %d]\n", __func__, __LINE__);

    // 设置videobuf
    vb->size = vcam_dev->vdata.format.fmt.pix.sizeimage;
    vb->bytesperline = vcam_dev->vdata.format.fmt.pix.bytesperline;
    vb->width = vcam_dev->vdata.format.fmt.pix.width;
    vb->height = vcam_dev->vdata.format.fmt.pix.height;
    vb->field = field;
    // 做些准备工作
    myvivi_precalculate_bars(0);
    // 调用videobuf_iolock类型为V4L2_MEMORY_USERPTR的videobuf分配内存
    // 设置状态
    vb->state = VIDEOBUF_PREPARED;

    return 0;
}

/*
 * App调用ioctl VIDIOC_QBUF时:
 * 1. 先调用buf_prepare进行一些准备工作
 * 2. 把buf放入队列
 * 3. 调用buf_queue(起通知作用)
 * 它会填充video_buffer结构体并调用videobuf_iolock来分配内存
 */

void myvivi_buffer_queue(struct videobuf_queue *vq, struct videobuf_buffer *vb)
{
    pr_info("Enter: %s [line: %d]\n", __func__, __LINE__);

    vb->state = VIDEOBUF_QUEUED;
    // 把videobuf放入到本地一个队列尾部，定时器处理函数就可以从本地队列取出videobuf
    list_add_tail(&vb->queue, &vcam_dev->vdata.vb_local_queue);
}

/*
 * App 不再使用队列时，用它来释放内存
 */
void myvivi_buffer_release(struct videobuf_queue *vq,         struct videobuf_buffer *vb)
{
    pr_info("Enter: %s [line: %d]\n", __func__, __LINE__);

    videobuf_vmalloc_free(vb);
    vb->state = VIDEOBUF_NEEDS_INIT;
}

