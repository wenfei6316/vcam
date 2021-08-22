#include "vcam_comm.h"
#include "v4l2_ops.h"
#include "buffer_queue_ops.h"

/*
 * ops->buf_setup   - calculates the size of the video buffers and avoid they
 *                    to waste more than some maximum limit of RAM;
 * ops->buf_prepare - fills the video buffer structs and calls
 *         videobuf_iolock() to alloc and prepare mmaped memory;
 * ops->buf_queue   - advices the driver that another buffer were
 *      requested (by read() or by QBUF);
 * ops->buf_release - frees any buffer that were allocated.
 *
 */
static struct videobuf_queue_ops vqops = {
    .buf_setup      = myvivi_buffer_setup, // 计算大小以免浪费
    .buf_prepare    = myvivi_buffer_prepare,
    .buf_queue      = myvivi_buffer_queue,
    .buf_release    = myvivi_buffer_release,
};

static struct mutex ext_lock;
int v4l2_open(struct file *file)
{
    pr_info("Enter: %s [line: %d]\n", __func__, __LINE__);
    // 队列操作2：初始化

    /*************************************************************************************/
        // 从file中获取dev数据 struct myvcam_device *vdev = video_drvdata(file);
        // 本次采用全局变量形式操作
    /*************************************************************************************/

    mutex_init(&ext_lock);
     videobuf_queue_vmalloc_init(&vcam_dev->vdata.vqueue, &vqops, NULL, &vcam_dev->queue_slock,
        V4L2_BUF_TYPE_VIDEO_CAPTURE, V4L2_FIELD_INTERLACED, sizeof(struct videobuf_buffer), NULL, &ext_lock);

    /*************************************************************************************/
        // 方案二采用线程操作
    /*************************************************************************************/

    // 新增定时器，通过定时器来唤醒buffer操作，也可以采用线程操作
    add_timer(&vcam_dev->timer);
    return 0;
}

unsigned int v4l2_poll(struct file *file, struct poll_table_struct *wait)
{
    pr_info("Enter: %s [line: %d]\n", __func__, __LINE__);

    return videobuf_poll_stream(file, &vcam_dev->vdata.vqueue, wait);
}

int v4l2_mmap(struct file *file, struct vm_area_struct *vma)
{
    pr_info("Enter: %s [line: %d]\n", __func__, __LINE__);
    return videobuf_mmap_mapper(&vcam_dev->vdata.vqueue, vma);
}

int v4l2_close(struct file *file)
{
    pr_info("Enter: %s [line: %d]\n", __func__, __LINE__);
    del_timer(&vcam_dev->timer);
    videobuf_stop(&vcam_dev->vdata.vqueue);
    videobuf_mmap_free(&vcam_dev->vdata.vqueue);
    return 0;
}

