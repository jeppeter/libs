#ifndef __MW_SDP_H__
#define __MW_SDP_H__

#include <time.h>

struct sdp_origin
{
    char *username;
    long long int sess_id;
    long long int sess_version;
    char *nettype;
    char *addrtype;
    char *addr;
};


struct sdp_connection
{
    char *nettype;
    char *addrtype;
    char *address;
};

struct sdp_bandwidth
{
    char *bwtype;
    char *bandwidth;
};

struct sdp_repeat
{
	time_t interval;
	time_t duration;
	time_t *offsets;
	size_t offsets_count;
};

struct sdp_time
{
	time_t start_time;
	time_t stop_time;
	struct sdp_repeat *repeat;
	size_t repeat_count;
};


struct sdp_zone_adjustments
{
	time_t adjust;
	time_t offset;
};


#define  SDP_VFORMAT_CODEC_FORMAT_MPEG4   1
#define  SDP_VFORMAT_CODEC_FORMAT_H264    2
#define  SDP_VFORMAT_CODEC_FORMAT_SVAC    3
#define  SDP_VFORMAT_CODEC_FORMAT_3GP     4

#define  SDP_VFORMAT_CODEC_FORMAT_MIN     1
#define  SDP_VFORMAT_CODEC_FORMAT_MAX     4


#define  SDP_VFORMAT_RESO_QCIF            1
#define  SDP_VFORMAT_RESO_CIF             2
#define  SDP_VFORMAT_RESO_4CIF            3
#define  SDP_VFORMAT_RESO_D1              4
#define  SDP_VFORMAT_RESO_720P            5
#define  SDP_VFORMAT_RESO_1080PI          6

#define  SDP_VFORMAT_RESO_MIN             1
#define  SDP_VFORMAT_RESO_MAX             6

#define  SDP_VFORMAT_FRAME_RATE_MIN       0
#define  SDP_VFORMAT_FRAME_RATE_MAX       99

#define  SDP_VFORMAT_BIT_RATE_CBR         1
#define  SDP_VFORMAT_BIT_RATE_VBR         2

#define  SDP_VFORMAT_BIT_RATE_MIN         1
#define  SDP_VFORMAT_BIT_RATE_MAX         2

#define  SDP_VFORMAT_KPS_RATE_MIN         0
#define  SDP_VFORMAT_KPS_RATE_MAX         100000



#define  SDP_AFORMAT_CODEC_FORMAT_G711    1
#define  SDP_AFORMAT_CODEC_FORMAT_G7231   2
#define  SDP_AFORMAT_CODEC_FORMAT_G729    3
#define  SDP_AFORMAT_CODEC_FORMAT_G7221   4

#define  SDP_AFORMAT_CODEC_FORMAT_MIN     1
#define  SDP_AFORMAT_CODEC_FORMAT_MAX     4

#define  SDP_AFORMAT_KPS_RATE_53          1
#define  SDP_AFORMAT_KPS_RATE_63          2
#define  SDP_AFORMAT_KPS_RATE_80          3
#define  SDP_AFORMAT_KPS_RATE_160         4
#define  SDP_AFORMAT_KPS_RATE_240         5
#define  SDP_AFORMAT_KPS_RATE_320         6
#define  SDP_AFORMAT_KPS_RATE_480         7
#define  SDP_AFORMAT_KPS_RATE_640         8

#define  SDP_AFORMAT_KPS_RATE_MIN         1
#define  SDP_AFORMAT_KPS_RATE_MAX         8

#define  SDP_AFORMAT_FREQ_RATE_80         1
#define  SDP_AFORMAT_FREQ_RATE_140        2
#define  SDP_AFORMAT_FREQ_RATE_160        3
#define  SDP_AFORMAT_FREQ_RATE_320        4

#define  SDP_AFORMAT_FREQ_RATE_MIN        1
#define  SDP_AFORMAT_FREQ_RATE_MAX        4


struct sdp_video_format
{
	int codec_format;
	int video_resolution;
	int frame_rate;
	int bit_rate;
	int kps_rate;
};

struct sdp_audio_format
{
	int codec_format;
	int kps_rate;
	int freq_rate;
};


struct sdp_info
{
	char *type;
	int port;
	int port_n;
	char *proto;
	int *fmt;
	size_t fmt_count;
};


struct sdp_media
{
	struct sdp_info info;
	char *title;
	struct sdp_connection conn;
	struct sdp_bandwidth *bw;
	size_t bw_count;
	char *encrypt_key;
	char **attributes;
	size_t attributes_count;
};


struct sdp_payload
{
    char *_payload;
	int alloc_size;
	int len_size;
    unsigned char proto_version;
    struct sdp_origin origin;
    char *session_name;
    char *information;
    char *uri;
    char **emails;
    size_t emails_count;
    char **phones;
    size_t phones_count;
    struct sdp_connection conn;
    struct sdp_bandwidth *bw;
    size_t bw_count;
    struct sdp_time *times;
    size_t times_count;
    struct sdp_zone_adjustments *zone_adjustments;
    size_t zone_adjustments_count;
    char *encrypt_key;
    char **attributes;
    size_t attributes_count;
    struct sdp_media *medias;
    size_t medias_count;
	char* ssrc;
	struct sdp_video_format vformat;
	struct sdp_audio_format aformat;

};

#ifdef __cplusplus
extern "C"
{
#endif
struct sdp_payload *sdp_parse(const char *payload);
void sdp_destroy(struct sdp_payload *sdp);
void sdp_dump(struct sdp_payload *sdp);

char *sdp_get_attr(char **attr, size_t nattr, char *key);
int sdp_has_flag_attr(char **attr, size_t nattr, char *flag);

#ifdef __cplusplus
}
#endif

#endif /*__MW_SDP_H__*/


