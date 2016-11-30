#ifndef VIDEODEVICE_H
#define VIDEODEVICE_H

#include <QString>
#include <QObject>
extern "C"
{
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <asm/types.h>
#include <linux/videodev2.h>
}

#define MAX_BUFFERS     (1)

struct video_buffer_t
{
    void * start;
    size_t length;
};

class VideoDevice : public QObject
{
    Q_OBJECT

public:
    VideoDevice(const char * name, int w, int h);
    int open_device(void);
    int close_device(void);
    int init_device(void);
    int uninit_device(void);
    int start_capturing(void);
    int stop_capturing(void);
    int get_frame(void ** frame_buf, size_t * len);
    int unget_frame(void);

private:
    int fd;
    char name[64];
    int width;
    int height;
    struct video_buffer_t * buffers;
    int n_buffers;
    int index;
};

#endif
