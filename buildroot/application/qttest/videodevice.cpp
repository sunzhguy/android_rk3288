#include "videodevice.h"
extern "C"
{
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>
}

VideoDevice::VideoDevice(const char * name, int w, int h)
{
	this->fd = -1;
	strcpy(this->name, name);
	this->buffers = NULL;
	this->n_buffers = 0;
	this->index = -1;
	this->width = w;
	this->height = h;
}

int VideoDevice::open_device(void)
{
    fd = open(name, O_RDWR, 0);

	if(fd < 0)
	{
		printf("open video device '%s' fail\n", name);
		return -1;
	}

	return 0;
}

int VideoDevice::close_device(void)
{
	if(close(fd) < 0)
	{
		printf("close video device '%s' fail\n", name);
		return -1;
	}

	return 0;
}

static int get_pixel_depth(unsigned int fmt)
{
	int depth = 0;

	switch (fmt)
	{
	case V4L2_PIX_FMT_NV12:
		depth = 12;
		break;
	case V4L2_PIX_FMT_NV21:
		depth = 12;
		break;
	case V4L2_PIX_FMT_YUV420:
		depth = 12;
		break;

	case V4L2_PIX_FMT_RGB565:
	case V4L2_PIX_FMT_YUYV:
	case V4L2_PIX_FMT_YVYU:
	case V4L2_PIX_FMT_UYVY:
	case V4L2_PIX_FMT_VYUY:
	case V4L2_PIX_FMT_NV16:
	case V4L2_PIX_FMT_NV61:
	case V4L2_PIX_FMT_YUV422P:
		depth = 16;
		break;

	case V4L2_PIX_FMT_RGB32:
		depth = 32;
		break;
	}

	return depth;
}

static int fimc_v4l2_querycap(int fp)
{
	struct v4l2_capability cap;
	int ret = 0;

	ret = ioctl(fp, VIDIOC_QUERYCAP, &cap);
	if (ret < 0)
	{
		printf("ERR(%s):VIDIOC_QUERYCAP failed\n", __func__);
		return -1;
	}

	if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
	{
		printf("ERR(%s):no capture devices\n", __func__);
		return -1;
	}

	return ret;
}

static const __u8 * fimc_v4l2_enuminput(int fp, int index)
{
	static struct v4l2_input input;

	input.index = index;
	if (ioctl(fp, VIDIOC_ENUMINPUT, &input) != 0)
	{
		printf("ERR(%s):No matching index found\n", __func__);
		return NULL;
	}

	printf("Name of input channel[%d] is %s\n", input.index, input.name);
	return input.name;
}

static int fimc_v4l2_s_input(int fp, int index)
{
	struct v4l2_input input;
	int ret;

	input.index = index;

	ret = ioctl(fp, VIDIOC_S_INPUT, &input);
	if (ret < 0)
	{
		printf("ERR(%s):VIDIOC_S_INPUT failed\n", __func__);
		return ret;
	}

	return ret;
}

static int fimc_v4l2_s_fmt(int fp, int width, int height, unsigned int fmt,	int flag_capture)
{
	struct v4l2_format v4l2_fmt;
	struct v4l2_pix_format pixfmt;

	memset(&pixfmt, 0, sizeof(pixfmt));
	memset(&v4l2_fmt, 0, sizeof(v4l2_format));

	v4l2_fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	pixfmt.width = width;
	pixfmt.height = height;
	pixfmt.pixelformat = fmt;
	if (fmt == V4L2_PIX_FMT_JPEG)
	{
		pixfmt.colorspace = V4L2_COLORSPACE_JPEG;
	}

	pixfmt.sizeimage = (width * height * get_pixel_depth(fmt)) / 8;

	if (flag_capture == 1)
		pixfmt.field = V4L2_FIELD_NONE;

	v4l2_fmt.fmt.pix = pixfmt;

	if (ioctl(fp, VIDIOC_S_FMT, &v4l2_fmt) < 0)
	{
		printf("ERR(%s):VIDIOC_S_FMT failed\n", __func__);
		return -1;
	}

	return 0;
}

static int fimc_v4l2_enum_fmt(int fp, unsigned int fmt)
{
	struct v4l2_fmtdesc fmtdesc;
	int found = 0;

	fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmtdesc.index = 0;

	while (ioctl(fp, VIDIOC_ENUM_FMT, &fmtdesc) == 0)
	{
		if (fmtdesc.pixelformat == fmt)
		{
			printf("passed fmt = %#x found pixel format[%d]: %s\n", fmt,
					fmtdesc.index, fmtdesc.description);
			found = 1;
			break;
		}

		fmtdesc.index++;
	}

	if (!found)
	{
		printf("unsupported pixel format\n");
		return -1;
	}

	return 0;
}

static int fimc_v4l2_reqbufs(int fp, enum v4l2_buf_type type, int nr_bufs)
{
	struct v4l2_requestbuffers req;
	int ret;

    memset(&req, 0, sizeof(struct v4l2_requestbuffers));

	req.count = nr_bufs;
	req.type = type;
	req.memory = V4L2_MEMORY_MMAP;

	ret = ioctl(fp, VIDIOC_REQBUFS, &req);
	if (ret < 0)
	{
		printf("ERR(%s):VIDIOC_REQBUFS failed\n", __func__);
		return -1;
	}

	return req.count;
}

static int fimc_v4l2_querybuf(int fp, struct video_buffer_t *buf, enum v4l2_buf_type type)
{
	struct v4l2_buffer v4l2_buf;
	int ret;

    memset(&v4l2_buf, 0, sizeof(struct v4l2_buffer));

	v4l2_buf.type = type;
	v4l2_buf.memory = V4L2_MEMORY_MMAP;
    v4l2_buf.index = 0;

	ret = ioctl(fp, VIDIOC_QUERYBUF, &v4l2_buf);
	if (ret < 0)
	{
		printf("ERR(%s):VIDIOC_QUERYBUF failed\n", __func__);
		return -1;
	}

	buf->length = v4l2_buf.length;
	if ((buf->start = (char *) mmap(0, v4l2_buf.length,
			PROT_READ | PROT_WRITE, MAP_SHARED, fp, v4l2_buf.m.offset)) < 0)
	{
		printf("[%s %d] mmap() failed\n", __func__, __LINE__);
		return -1;
	}

	printf("%s: buf->start = %p v4l2_buf.length = %d\n", __func__, buf->start, v4l2_buf.length);

	return 0;
}

static int fimc_v4l2_streamon(int fp)
{
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	int ret;

	ret = ioctl(fp, VIDIOC_STREAMON, &type);
	if (ret < 0)
	{
		printf("ERR(%s):VIDIOC_STREAMON failed\n", __func__);
		return ret;
	}

	return ret;
}

static int fimc_v4l2_streamoff(int fp)
{
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	int ret;

	ret = ioctl(fp, VIDIOC_STREAMOFF, &type);
	if (ret < 0)
	{
		printf("ERR(%s):VIDIOC_STREAMOFF failed\n", __func__);
		return ret;
	}

	return ret;
}

static int fimc_v4l2_qbuf(int fp, int index)
{
	struct v4l2_buffer v4l2_buf;
	int ret;

    memset(&v4l2_buf, 0, sizeof(struct v4l2_buffer));

	v4l2_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	v4l2_buf.memory = V4L2_MEMORY_MMAP;
	v4l2_buf.index = index;

	ret = ioctl(fp, VIDIOC_QBUF, &v4l2_buf);
	if (ret < 0)
	{
		printf("ERR(%s):VIDIOC_QBUF failed\n", __func__);
		return ret;
	}

	return 0;
}

static int fimc_v4l2_dqbuf(int fp)
{
	struct v4l2_buffer v4l2_buf;
	int ret;

	memset(&v4l2_buf, 0, sizeof(v4l2_buffer));

	v4l2_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	v4l2_buf.memory = V4L2_MEMORY_MMAP;

	ret = ioctl(fp, VIDIOC_DQBUF, &v4l2_buf);
	if (ret < 0)
	{
		printf("ERR(%s):VIDIOC_DQBUF failed, dropped frame\n", __func__);
		return ret;
	}

	return v4l2_buf.index;
}

static int fimc_v4l2_g_ctrl(int fp, unsigned int id)
{
	struct v4l2_control ctrl;
	int ret;

	ctrl.id = id;

	ret = ioctl(fp, VIDIOC_G_CTRL, &ctrl);
	if (ret < 0)
	{
		printf("ERR(%s): VIDIOC_G_CTRL(id = 0x%x (%d)) failed, ret = %d\n", __func__, id, id - V4L2_CID_PRIVATE_BASE, ret);
		return ret;
	}

	return ctrl.value;
}

static int fimc_v4l2_s_ctrl(int fp, unsigned int id, unsigned int value)
{
	struct v4l2_control ctrl;
	int ret;

	ctrl.id = id;
	ctrl.value = value;

	ret = ioctl(fp, VIDIOC_S_CTRL, &ctrl);
	if (ret < 0)
	{
		printf("ERR(%s):VIDIOC_S_CTRL(id = %#x (%d), value = %d) failed ret = %d\n", __func__, id, id - V4L2_CID_PRIVATE_BASE, value, ret);

		return ret;
	}

	return ctrl.value;
}

static int fimc_v4l2_s_ext_ctrl(int fp, unsigned int id, void *value)
{
	struct v4l2_ext_controls ctrls;
	struct v4l2_ext_control ctrl;
	int ret;

	ctrl.id = id;

	ctrls.ctrl_class = V4L2_CTRL_CLASS_CAMERA;
	ctrls.count = 1;
	ctrls.controls = &ctrl;

	ret = ioctl(fp, VIDIOC_S_EXT_CTRLS, &ctrls);
	if (ret < 0)
		printf("ERR(%s):VIDIOC_S_EXT_CTRLS failed\n", __func__);

	return ret;
}

static int fimc_v4l2_g_parm(int fp, struct v4l2_streamparm *streamparm)
{
	int ret;

	streamparm->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	ret = ioctl(fp, VIDIOC_G_PARM, streamparm);
	if (ret < 0)
	{
		printf("ERR(%s):VIDIOC_G_PARM failed\n", __func__);
		return -1;
	}

	printf("%s : timeperframe: numerator %d, denominator %d\n", __func__,
			streamparm->parm.capture.timeperframe.numerator,
			streamparm->parm.capture.timeperframe.denominator);

	return 0;
}

static int fimc_v4l2_s_parm(int fp, struct v4l2_streamparm *streamparm)
{
	int ret;

	streamparm->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	ret = ioctl(fp, VIDIOC_S_PARM, streamparm);
	if (ret < 0)
	{
		printf("ERR(%s):VIDIOC_S_PARM failed\n", __func__);
		return ret;
	}

	return 0;
}

int VideoDevice::init_device(void)
{
	if (fimc_v4l2_querycap(fd) < 0)
		return -1;
	if (!fimc_v4l2_enuminput(fd, 0))
		return -1;
	if (fimc_v4l2_s_input(fd, 0))
		return -1;

	if(fimc_v4l2_enum_fmt(fd, V4L2_PIX_FMT_RGB565) < 0)
		return -1;
    if(fimc_v4l2_s_fmt(fd, width, height, V4L2_PIX_FMT_RGB565, 0) < 0)
    	return -1;

    /*
     * memory map
     */
    if(fimc_v4l2_reqbufs(fd, V4L2_BUF_TYPE_VIDEO_CAPTURE, MAX_BUFFERS) < 0)
    	return -1;

    buffers = (struct video_buffer_t *)calloc(MAX_BUFFERS, sizeof(struct video_buffer_t));
    if(!buffers)
    {
    	printf("out of memory\n");
        return -1;
    }

    for(n_buffers = 0; n_buffers < MAX_BUFFERS; n_buffers++)
    {
        if(fimc_v4l2_querybuf(fd, &buffers[n_buffers], V4L2_BUF_TYPE_VIDEO_CAPTURE) < 0)
    		return -1;
    }

    return 0;
}

int VideoDevice::uninit_device(void)
{
	int i;

	for(i = 0; i < n_buffers; i++)
	{
		if (munmap(buffers[i].start, buffers[i].length) < 0)
		{
			printf("munmap:[%d] fail\n", i);
			return -1;
		}

	}

	free(buffers);
	return 0;
}

int VideoDevice::start_capturing(void)
{
	struct v4l2_streamparm m_streamparm;
	int i;

	/*
	 * start with all buffers in queue
	 */
	for (i = 0; i < n_buffers; i++)
	{
		if(fimc_v4l2_qbuf(fd, i) < 0)
			return -1;
	}

	if(fimc_v4l2_streamon(fd) < 0)
		return -1;

	memset(&m_streamparm, 0, sizeof(struct v4l2_streamparm));
	m_streamparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    m_streamparm.parm.capture.timeperframe.numerator = 1;
    m_streamparm.parm.capture.timeperframe.denominator = 15;

	if(fimc_v4l2_s_parm(fd, &m_streamparm) < 0)
		return -1;

	fimc_v4l2_g_parm(fd, &m_streamparm);
	return 0;
}

int VideoDevice::stop_capturing(void)
{
	return fimc_v4l2_streamoff(fd);
}

int VideoDevice::get_frame(void ** frame_buf, size_t * len)
{
	int i;

	i = fimc_v4l2_dqbuf(fd);
	if(i < 0)
		return -1;

	*frame_buf = buffers[i].start;
    *len = buffers[i].length;
    index = i;

    return 0;
}

int VideoDevice::unget_frame(void)
{
    if(index >= 0)
    	return fimc_v4l2_qbuf(fd, index);
    return -1;
}
