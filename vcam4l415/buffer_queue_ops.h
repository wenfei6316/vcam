#ifndef __BUFFER_QUEUE_OPS_H__
#define __BUFFER_QUEUE_OPS_H__

#include "vcam_comm.h"

int myvivi_buffer_setup(struct videobuf_queue *q, unsigned int *count, unsigned int *size);
int myvivi_buffer_prepare(struct videobuf_queue *vq, struct videobuf_buffer *vb, enum v4l2_field field);
int myvivi_buffer_prepare(struct videobuf_queue *vq, struct videobuf_buffer *vb, enum v4l2_field field);
void myvivi_buffer_queue(struct videobuf_queue *vq, struct videobuf_buffer *vb);
void myvivi_buffer_release(struct videobuf_queue *vq,         struct videobuf_buffer *vb);



#endif