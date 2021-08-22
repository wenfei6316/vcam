#ifndef __V4L2_OPS_H__
#define __V4L2_OPS_H__

#include "vcam_comm.h"

int myvivi_vidioc_querycap(struct file *file, void  *priv, struct v4l2_capability *cap);
int myvivi_vidioc_enum_fmt_vid_cap(struct file *file, void  *priv, struct v4l2_fmtdesc *f);
int myvivi_vidioc_g_fmt_vid_cap(struct file *file, void *priv, struct v4l2_format *f);
int myvivi_vidioc_try_fmt_vid_cap(struct file *file, void *priv, struct v4l2_format *f);
int myvivi_vidioc_s_fmt_vid_cap(struct file *file, void *priv, struct v4l2_format *f);
int myvivi_vidioc_reqbufs(struct file *file, void *priv, struct v4l2_requestbuffers *p);
int myvivi_vidioc_querybuf(struct file *file, void *priv, struct v4l2_buffer *p);
int myvivi_vidioc_qbuf(struct file *file, void *priv, struct v4l2_buffer *p);
int myvivi_vidioc_dqbuf(struct file *file, void *priv, struct v4l2_buffer *p);
int myvivi_vidioc_streamon(struct file *file, void *priv, enum v4l2_buf_type i);
int myvivi_vidioc_streamoff(struct file *file, void *priv, enum v4l2_buf_type i);

int v4l2_open(struct file *file);
unsigned int v4l2_poll(struct file *file, struct poll_table_struct *wait);
int v4l2_mmap(struct file *file, struct vm_area_struct *vma);
int v4l2_close(struct file *file);


#endif

