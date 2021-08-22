#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#include <linux/videodev2.h>


int main(int argc, const char *argv[])
{
    //1.打开设备
    int ret;
    int fd = open("/dev/video0", O_RDWR);
    if(fd < 0) {
        printf("Enter: %s [line: %d],打开设备失败", __func__, __LINE__);
        return -1;
    }

    // 2.获取摄像头支持多少种格式，以及格式类型
    struct v4l2_fmtdesc v4fmt;
    v4fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    int i = 0;
    for (; ;) {
        v4fmt.index = i++;
        ret = ioctl(fd, VIDIOC_ENUM_FMT, &v4fmt);
        if(ret < 0) {
            printf("Enter: %s [line: %d], 总过有%d种格式\n", __func__, __LINE__, v4fmt.index);
            break;
        }
        printf("v4fmt.description = %s, pixelformat = 0x%X, V4L2_PIX_FMT_YUYV = 0x%X\n", v4fmt.description, v4fmt.pixelformat, V4L2_PIX_FMT_YUYV);
    }

    // 3.设置采集格式
    struct v4l2_format vfmt;
    vfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    vfmt.fmt.pix.width = 640;
    vfmt.fmt.pix.height = 480;
    vfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    vfmt.fmt.pix.field = V4L2_FIELD_ANY;
    ret = ioctl(fd, VIDIOC_S_FMT, &vfmt);
    if (ret < 0) {
        printf("Enter: %s [line: %d],设置格式失败\n", __func__, __LINE__);
    }
    memset(&vfmt, 0, sizeof(vfmt));
    vfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ret  = ioctl(fd, VIDIOC_G_FMT, &vfmt);
    if (ret < 0) {
        printf("Enter: %s [line: %d],获取格式失败\n", __func__, __LINE__);
    }
    if(vfmt.fmt.pix.width == 640 && vfmt.fmt.pix.height == 480 && vfmt.fmt.pix.pixelformat == V4L2_PIX_FMT_YUYV) {
        printf("Enter: %s [line: %d],设置成功\n", __func__, __LINE__);
    } else {
        printf("Enter: %s [line: %d],设置失败\n", __func__, __LINE__);
    }
    printf("vfmt.fmt.pix.width = %d, vfmt.fmt.pix.height = %d, vfmt.fmt.pix.pixelformat = 0x%X, V4L2_PIX_FMT_YUYV = 0x%X\n",
        vfmt.fmt.pix.width, vfmt.fmt.pix.height, vfmt.fmt.pix.pixelformat, V4L2_PIX_FMT_YUYV);

    //9.关闭设备
    close(fd);
    return 0;
}


