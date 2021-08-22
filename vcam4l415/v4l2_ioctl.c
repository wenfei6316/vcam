#include "vcam_comm.h"
#include "v4l2_ioctl.h"

#define DEBUG_TEST

/* ------------------------------------------------------------------
    IOCTL vidioc handling
   ------------------------------------------------------------------*/
int myvivi_vidioc_querycap(struct file *file, void  *priv,
                    struct v4l2_capability *cap)
{
    pr_info("Enter: %s [line: %d]\n", __func__, __LINE__);
    strcpy(cap->driver, "myvivi");
    strcpy(cap->card, "myvivi");
    cap->version = 0x0001;
    strcpy(cap->bus_info, "platform: virtual");
    cap->capabilities = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING | V4L2_CAP_READWRITE | V4L2_CAP_DEVICE_CAPS;
    cap->device_caps = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING | V4L2_CAP_READWRITE;
    return 0;
}

// 列举支持哪种格式
/* 该函数是在应用层通过ioctl(fd, VIDIOC_ENUM_FMT, &v4fmt)调用时执行的
 * 通过该函数实现以下几个功能：
 * 1、我们打开的videoX可能支持很多种格式，我们通过f->index也就是v4fmt.index来表示我们要获取第index是哪种格式
 * 2、f->description描述的是第index种格式的名称
 * 3、f->pixelformat描述的是第index种格式的像素
 */
int myvivi_vidioc_enum_fmt_vid_cap(struct file *file, void  *priv, struct v4l2_fmtdesc *f)
{
    pr_info("Enter: %s [line: %d]\n", __func__, __LINE__);
    if (f->index >= 1)
        return -EINVAL;

    // 设置vivi的图片描述，从结构体struct vivi_fmt中选择合适的
    strcpy(f->description, "4:2:2, packed, YUYV");
    // 设置vivi的图片格式，从结构体struct vivi_fmt中选择合适的
    f->pixelformat = V4L2_PIX_FMT_YUYV;
    return 0;
}

// 返回当前所使用的格式
/* 该函数是在应用层通过ioctl(fd, VIDIOC_G_FMT, &v4fmt)调用时执行的
 * 通过该函数实现以下几个功能：
 * 1、将当前设备设置的格式传给应用层
 */
int myvivi_vidioc_g_fmt_vid_cap(struct file *file, void *priv, struct v4l2_format *f)
{
    pr_info("Enter: %s [line: %d]\n", __func__, __LINE__);
    memcpy(f, &vcam_dev->vdata.format, sizeof(struct v4l2_format));

    return (0);
}

// 测试驱动程序是否支持某种格式
/* 该函数是在应用层通过ioctl(fd, VIDIOC_TRY_FMT, &v4fmt)调用时执行的
 * 通过该函数实现以下几个功能：
 * 1、判断传入的格式设备是否支持
 * 2、设置field值，可参考https://blog.csdn.net/kickxxx/article/details/6367669
 * 3、设置图片的width、height、bytesperline、sizeimage等图片格式
 */
int myvivi_vidioc_try_fmt_vid_cap(struct file *file, void *priv, struct v4l2_format *f)
{
    pr_info("Enter: %s [line: %d]\n", __func__, __LINE__);
    unsigned int maxw, maxh;
    enum v4l2_field field;

    // 检查设备是否支持当前传入的格式
    if (f->fmt.pix.pixelformat != V4L2_PIX_FMT_YUYV)
        return -EINVAL;

    field = f->fmt.pix.field;

    if (field == V4L2_FIELD_ANY) {
        field = V4L2_FIELD_INTERLACED;
    } else if (V4L2_FIELD_INTERLACED != field) {
        return -EINVAL;
    }

    maxw  = 1024;
    maxh  = 768;

    // 调整format的width,height, 计算bytesperline和sizeimage
    f->fmt.pix.field = field;
    v4l_bound_align_image(&f->fmt.pix.width, 48, maxw, 2, &f->fmt.pix.height, 32, maxh, 0, 0);
    f->fmt.pix.bytesperline = (f->fmt.pix.width * 16) >> 3;
    f->fmt.pix.sizeimage = f->fmt.pix.height * f->fmt.pix.bytesperline;

    return 0;
}


// 设置格式并保存到format中
/* 该函数是在应用层通过ioctl(fd, VIDIOC_S_FMT, &v4fmt)调用时执行的
 * 通过该函数实现以下几个功能：
 * 1、设置图片格式
 * 2、图片信息保存到设备中
 */
int myvivi_vidioc_s_fmt_vid_cap(struct file *file, void *priv, struct v4l2_format *f)
{
    pr_info("Enter: %s [line: %d]\n", __func__, __LINE__);
    int ret = myvivi_vidioc_try_fmt_vid_cap(file, NULL, f);
    if (ret < 0)
        return ret;

    memcpy(&vcam_dev->vdata.format, f, sizeof(struct v4l2_format));

    return ret;
}

int myvivi_vidioc_reqbufs(struct file *file, void *priv, struct v4l2_requestbuffers *p)
{
    pr_info("Enter: %s [line: %d]\n", __func__, __LINE__);
    return (videobuf_reqbufs(&vcam_dev->vdata.vqueue, p));
}

int myvivi_vidioc_querybuf(struct file *file, void *priv, struct v4l2_buffer *p)
{
    pr_info("Enter: %s [line: %d]\n", __func__, __LINE__);
    return (videobuf_querybuf(&vcam_dev->vdata.vqueue, p));
}

int myvivi_vidioc_qbuf(struct file *file, void *priv, struct v4l2_buffer *p)
{
    pr_info("Enter: %s [line: %d]\n", __func__, __LINE__);
    return (videobuf_qbuf(&vcam_dev->vdata.vqueue, p));
}

int myvivi_vidioc_dqbuf(struct file *file, void *priv, struct v4l2_buffer *p)
{
    pr_info("Enter: %s [line: %d]\n", __func__, __LINE__);
    return (videobuf_dqbuf(&vcam_dev->vdata.vqueue, p, file->f_flags & O_NONBLOCK));
}

int myvivi_vidioc_streamon(struct file *file, void *priv, enum v4l2_buf_type i)
{
    pr_info("Enter: %s [line: %d]\n", __func__, __LINE__);
    return videobuf_streamon(&vcam_dev->vdata.vqueue);
}

int myvivi_vidioc_streamoff(struct file *file, void *priv, enum v4l2_buf_type i)
{
    pr_info("Enter: %s [line: %d]\n", __func__, __LINE__);
    return videobuf_streamoff(&vcam_dev->vdata.vqueue);
}

