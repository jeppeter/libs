#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <stdarg.h>
#include "sdp.h"
#include <errno.h>

#define ERROR(...) do{fprintf(stderr,"[%s:%d]\t",__FILE__,__LINE__);fprintf(stderr,__VA_ARGS__);}while(0)

static char *load_next_entry(char *p, char *key, char **value)
{
    char *endl;

    if (!p)
        goto fail;

    endl = strstr(p, "\r\n");
    if (!endl)
        endl = strchr(p, '\n');

    if (endl)
        while (*endl == '\r' || *endl == '\n')
            *endl++ = '\0';
    else
        endl = &p[strlen(p)];

    if (!p[0] || p[1] != '=')
        goto fail;

    *key = p[0];
    *value = &p[2];

    return endl;

fail:
    *key   = 0;
    *value = NULL;
    return NULL;
}

static char *split_values(char *p, char sep, char *fmt, ...)
{
    va_list va;

    va_start(va, fmt);
    while (*p == sep)
        p++;
    while (*fmt) {
        char **s, *tmp;
        int *i;
        long long int *l;
        time_t *t;

        switch (*fmt++) {
        case 's':
            s = va_arg(va, char **);
            *s = p;
            tmp = strchr(p, sep);
            if (tmp) {
                p = tmp;
                while (*p == sep)
                    *p++ = '\0';
            } else {
                p = &p[strlen(p)];
            }
            break;
        case 'l':
            l = va_arg(va, long long int *);
            *l = strtoll(p, &tmp, 10);
            if (tmp == p)
                *p = 0;
            else
                p = tmp;
            break;
        case 'i':
            i = va_arg(va, int *);
            *i = strtol(p, &tmp, 10);
            if (tmp == p)
                *p = 0;
            else
                p = tmp;
            break;
        case 't':
            t = va_arg(va, time_t *);
            *t = strtol(p, &tmp, 10);
            if (tmp == p) {
                *p = 0;
            } else {
                p = tmp;
                switch (*p) {
                case 'd': *t *= 86400; p++; break;
                case 'h': *t *=  3600; p++; break;
                case 'm': *t *=    60; p++; break;
                }
            }
            break;
        }
        while (*p == sep)
            p++;
    }
    va_end(va);
    return p;
}


static char* split_values_withempty(char* p,char sep,int *pNumparse,char*fmt,...)
{
	 va_list va;
	 int numparse=0;
	
	 va_start(va, fmt);

	 
	 while (*fmt) {
		 char  *tmp,*s;
		 int *i;
	
		 switch (*fmt++)
		 {
		 case 'c':
		 	s = va_arg(va,char *);
			*s = *p;
			p ++;
			numparse ++;
		 	break;
		 case 'i':
			 i = va_arg(va, int *);
			 *i = strtol(p, &tmp, 10);
			 if (tmp != p)
				 p = tmp;
			 numparse ++;
			 break;
		 }
		 if (*p == sep)
		 {
		 	p ++;
		 }

		 if (*p == '\0')
		 {
		 	break;
		 }
	 }
	 va_end(va);
	 *pNumparse = numparse;
	 return p;
	
}

#define GET_CONN_INFO(connf_ptr) do {                              \
    if (key == 'c') {                                              \
        struct sdp_connection *c = connf_ptr;                      \
        split_values(value, ' ', "sss", &c->nettype, &c->addrtype, \
                     &c->address);                                 \
        p = load_next_entry(p, &key, &value);                      \
    }                                                              \
} while (0)

#define GET_BANDWIDTH_INFO(bw) do {                                \
    int n;                                                         \
    while (key == 'b') {                                           \
        ADD_ENTRY(bw);                                             \
        n = bw ## _count - 1;                                      \
        split_values(value, ':', "ss", &bw[n].bwtype,              \
                     &bw[n].bandwidth);                            \
        p = load_next_entry(p, &key, &value);                      \
    }                                                              \
} while (0)

#define LOAD_FACULTATIVE_STR(k, field) do {                        \
    if (key == k) {                                                \
        field = value;                                             \
        p = load_next_entry(p, &key, &value);                      \
    }                                                              \
} while (0)

#define LOAD_MULTIPLE_FACULTATIVE_STR(k, field) do {               \
    while (key == k) {                                             \
        ADD_ENTRY(field);                                          \
        field[field ## _count - 1] = value;                        \
        p = load_next_entry(p, &key, &value);                      \
    }                                                              \
} while (0)

#define ADD_ENTRY(field) do {                                      \
    field ## _count++;                                             \
    if (!field) {                                                  \
        field = calloc(1, sizeof(*field));                         \
    } else {                                                       \
        int n = field ## _count;                                   \
        field = realloc(field, sizeof(*field) * n);                \
        memset(&field[n - 1], 0, sizeof(*field));                  \
    }                                                              \
    if (!(field))                                                  \
        goto fail;                                                 \
} while (0)



static int check_vformat(struct sdp_video_format *pVformat)
{
	/*this is default one*/
	if (pVformat->codec_format == 0)
	{
		if (pVformat->video_resolution != 0)
		{
			return 0;
		}
		if (pVformat->frame_rate != 0)
		{
			return 0;
		}

		if (pVformat->bit_rate != 0)
		{
			return 0;
		}

		if (pVformat->kps_rate != 0)
		{
			return 0;
		}
	}
	else
	{
		if (pVformat->codec_format < SDP_VFORMAT_CODEC_FORMAT_MIN ||
			pVformat->codec_format > SDP_VFORMAT_CODEC_FORMAT_MAX)
		{
			return 0;
		}

		if (pVformat->video_resolution < SDP_VFORMAT_RESO_MIN ||
			pVformat->video_resolution > SDP_VFORMAT_RESO_MAX)
		{
			return 0;
		}

		if (pVformat->frame_rate < SDP_VFORMAT_FRAME_RATE_MIN ||
			pVformat->frame_rate > SDP_VFORMAT_FRAME_RATE_MAX)
		{
			return 0;
		}
		if (pVformat->bit_rate < SDP_VFORMAT_BIT_RATE_MIN|| 
			pVformat->bit_rate > SDP_VFORMAT_BIT_RATE_MAX)
		{
			return 0;
		}

		if (pVformat->kps_rate < SDP_VFORMAT_KPS_RATE_MIN ||
			pVformat->kps_rate > SDP_VFORMAT_KPS_RATE_MAX)
		{
			return 0;
		}

	}

	return 1;
}


static int check_aformat(struct sdp_audio_format* pAformat)
{
	if (pAformat->codec_format == 0)
	{
		if (pAformat->kps_rate != 0)
		{
			return 0;
		}

		if (pAformat->freq_rate != 0)
		{
			return 0;
		}
	}
	else
	{
		if (pAformat->codec_format < SDP_AFORMAT_CODEC_FORMAT_MIN || 
			pAformat->codec_format > SDP_AFORMAT_CODEC_FORMAT_MAX)
		{
			return 0;
		}

		if (pAformat->kps_rate < SDP_AFORMAT_KPS_RATE_MIN ||
			pAformat->kps_rate > SDP_AFORMAT_KPS_RATE_MAX)
		{
			return 0;
		}

		if (pAformat->freq_rate < SDP_AFORMAT_FREQ_RATE_MIN || 
			pAformat->freq_rate > SDP_AFORMAT_FREQ_RATE_MAX)
		{
			return 0;
		}
	}
	return 1;
}

struct sdp_payload *sdp_parse(const char *payload)
{
    struct sdp_payload *sdp = calloc(1, sizeof(*sdp));
    char *p, key, *value;
	char vc,ac;
	int numparse;
	

    if (!sdp)
        goto fail;

    p = sdp->_payload = strdup(payload);
    if (!p)
        goto fail;


    /* Protocol version (mandatory, only 0 supported) */
    p = load_next_entry(p, &key, &value);
    if (key != 'v')
        goto fail;
    sdp->proto_version = value[0] - '0';
    if (sdp->proto_version != 0 || value[1])
        goto fail;

    /* Origin field (mandatory) */
    p = load_next_entry(p, &key, &value);
    if (key != 'o')
        goto fail;
    else {
        struct sdp_origin *o = &sdp->origin;
        split_values(value, ' ', "sllsss", &o->username, &o->sess_id,
                     &o->sess_version, &o->nettype, &o->addrtype, &o->addr);
    }

    /* Session name field (mandatory) */
    p = load_next_entry(p, &key, &value);
    if (key != 's')
        goto fail;
    sdp->session_name = value;
    p = load_next_entry(p, &key, &value);

    /* Information field */
    LOAD_FACULTATIVE_STR('i', sdp->information);

    /* URI field */
    LOAD_FACULTATIVE_STR('u', sdp->uri);

    /* Email addresses */
    LOAD_MULTIPLE_FACULTATIVE_STR('e', sdp->emails);

    /* Phone numbers */
    LOAD_MULTIPLE_FACULTATIVE_STR('p', sdp->phones);

    /* Connection information */
    GET_CONN_INFO(&sdp->conn);

    /* Bandwidth fields */
    GET_BANDWIDTH_INFO(sdp->bw);

    /* Time fields (at least one mandatory) */
    do {
        struct sdp_time *tf;

        ADD_ENTRY(sdp->times);
        tf = &sdp->times[sdp->times_count - 1];
        split_values(value, ' ', "tt", &tf->start_time, &tf->stop_time);
        p = load_next_entry(p, &key, &value);

        while (key == 'r') {
            struct sdp_repeat *rf;

            ADD_ENTRY(tf->repeat);
            rf = &tf->repeat[tf->repeat_count - 1];
            value = split_values(value, ' ', "tt", &rf->interval, &rf->duration);
            while (*value) {
                int n = rf->offsets_count;
                ADD_ENTRY(rf->offsets);
                value = split_values(value, ' ', "t", &rf->offsets[n]);
            }
            p = load_next_entry(p, &key, &value);
        }
    } while (key == 't');

    /* Zone adjustments */
    if (key == 'z') {
        while (*value) {
            int n = sdp->zone_adjustments_count;
            struct sdp_zone_adjustments *za;

            ADD_ENTRY(sdp->zone_adjustments);
            za = &sdp->zone_adjustments[n];
            value = split_values(value, ' ', "tt", &za->adjust, &za->offset);
        }
        p = load_next_entry(p, &key, &value);
    }

    /* Encryption key */
    LOAD_FACULTATIVE_STR('k', sdp->encrypt_key);

    /* Media attributes */
    LOAD_MULTIPLE_FACULTATIVE_STR('a', sdp->attributes);

    /* Media descriptions */
    while (key == 'm') {
        struct sdp_media *md;

        ADD_ENTRY(sdp->medias);
        md = &sdp->medias[sdp->medias_count - 1];

        value = split_values(value, ' ', "s", &md->info.type);
        md->info.port = strtol(value, &value, 10);
        md->info.port_n = *value == '/' ? strtol(value + 1, &value, 10) : 0;
        value = split_values(value, ' ', "s", &md->info.proto);
        while (*value) {
            ADD_ENTRY(md->info.fmt);
            value = split_values(value, ' ', "i", &md->info.fmt[md->info.fmt_count - 1]);
        }
        p = load_next_entry(p, &key, &value);

        LOAD_FACULTATIVE_STR('i', md->title);
        GET_CONN_INFO(&md->conn);
        GET_BANDWIDTH_INFO(md->bw);
        LOAD_FACULTATIVE_STR('k', md->encrypt_key);
        LOAD_MULTIPLE_FACULTATIVE_STR('a', md->attributes);
    }

	if (key == 'y')
	{
		/*now to get the key of the ssrcs*/
		sdp->ssrc = value;
		p = load_next_entry(p,&key,&value);
	}

	if (key == 'f')
	{
		if (*value != 'v')
		{
			goto skip_frame_format;
		}
		/*now to get the */
		value = split_values_withempty(value,'/',&numparse,"ciiiiiciii",
			&vc,
			&(sdp->vformat.codec_format),  /*codec format 1 for MPEG-4,2 for H.264 3 for SVAC 4 for 3GP*/
			&(sdp->vformat.video_resolution), /*resolution 1 for QCIF 2 for CIF 3 for 4CIF 4 for D1 5 for 720P 6 for 1080P/I*/
			&(sdp->vformat.frame_rate),      /*frame rate 0~99*/
			&(sdp->vformat.bit_rate),        /*1 for CBR (constant bitrate) 2 for VBR (variable bit rate)*/
			&(sdp->vformat.kps_rate),             /*0~100000 for kps  1 for 1kps 2 for 2kps*/
			&ac,
			&(sdp->aformat.codec_format),   /*1 for G.711 2 for G.723.1 3 for G.729 4 for G.722.1*/
			&(sdp->aformat.kps_rate),       /*1 for 5.3kps ,2 for 6.3kps ,3 for 8kps  ,4 for 16kps  ,5 for 24kps  ,6  for 32kps , 7 for 48kps ,8 for 64kps*/
			&(sdp->aformat.freq_rate)     /*1 for 8kps ,2 for 14kps  , 3  for 16kps  , 4  for 32kps*/
			);

		if (numparse != 10)
		{
			ERROR("num %d\n",numparse);
			goto fail;
		}
		if (ac != 'a' || vc != 'v')
		{
			ERROR("not valid char\n");
			goto fail;
		}


skip_frame_format:
		/*now to check*/
		if (check_vformat(&(sdp->vformat))==0)
		{
			ERROR("\n");
			goto fail;
		}

		if (check_aformat(&(sdp->aformat))==0)
		{
			ERROR("\n");
			goto fail;
		}

		
	}
	

    return sdp;

fail:
    sdp_destroy(sdp);
    return NULL;
}

void sdp_destroy(struct sdp_payload *sdp)
{
    size_t i, j;

    if (sdp) {
        free(sdp->_payload);
        free(sdp->emails);
        free(sdp->phones);
        free(sdp->bw);
        for (i = 0; i < sdp->times_count; i++) {
            for (j = 0; j < sdp->times[i].repeat_count; j++)
                free(sdp->times[i].repeat[j].offsets);
            free(sdp->times[i].repeat);
        }
        free(sdp->times);
        free(sdp->zone_adjustments);
        free(sdp->attributes);
        for (i = 0; i < sdp->medias_count; i++) {
            free(sdp->medias[i].info.fmt);
            free(sdp->medias[i].bw);
            free(sdp->medias[i].attributes);
        }
        free(sdp->medias);
    }
    free(sdp);
}

char *sdp_get_attr(char **attr, size_t nattr, char *key)
{
    size_t i, klen = strlen(key);

    for (i = 0; i < nattr; i++)
        if (!strncmp(attr[i], key, klen) && attr[i][klen] == ':')
            return &attr[i][klen + 1];
    return NULL;
}

int sdp_has_flag_attr(char **attr, size_t nattr, char *flag)
{
    size_t i;

    for (i = 0; i < nattr; i++)
        if (!strcmp(attr[i], flag))
            return 1;
    return 0;
}

#define BUFFER_SNPRINTF(...) \
do\
{\
	ret = snprintf(pCurPtr,leftlen,__VA_ARGS__);\
	if (ret < 0)\
	{\
		return -2;\
	}\
	if (ret == leftlen)\
	{\
		return -1;\
	}\
	retsize += ret;\
	pCurPtr += ret;\
	leftlen -= ret;\
}while(0)

/*
  -1 for no buffer
  -2 for other error
*/
static int __sdp_dump(struct sdp_payload *sdp,char* pBuffer,int len)
{
	int ret;
	char* pCurPtr=pBuffer;
	int leftlen = len;
	int retsize= 0;
	int i,j,k;

	BUFFER_SNPRINTF("v=%d\n", sdp->proto_version);
	
	BUFFER_SNPRINTF("o=%s %lld %lld %s %s %s\n", sdp->origin.username,
		   sdp->origin.sess_id, sdp->origin.sess_version, sdp->origin.nettype,
		   sdp->origin.addrtype, sdp->origin.addr);
	BUFFER_SNPRINTF("s=%s\n", sdp->session_name);

	if (sdp->information) BUFFER_SNPRINTF("i=%s\n", sdp->information);
	if (sdp->uri)		  BUFFER_SNPRINTF("u=%s\n", sdp->uri);

	for (i = 0; i < sdp->emails_count; i++) BUFFER_SNPRINTF("e=%s\n", sdp->emails[i]);
	for (i = 0; i < sdp->phones_count; i++) BUFFER_SNPRINTF("p=%s\n", sdp->phones[i]);

	if (sdp->conn.nettype && sdp->conn.addrtype && sdp->conn.address)
		BUFFER_SNPRINTF("c=%s %s %s\n",
			   sdp->conn.nettype, sdp->conn.addrtype, sdp->conn.address);

	for (i = 0; i < sdp->bw_count; i++)
		BUFFER_SNPRINTF("b=%s:%s\n", sdp->bw[i].bwtype, sdp->bw[i].bandwidth);

	for (i = 0; i < sdp->times_count; i++) {
		struct sdp_time *t = &sdp->times[i];
		BUFFER_SNPRINTF("t=%ld %ld\n", t->start_time, t->stop_time);
		for (j = 0; j < t->repeat_count; j++) {
			struct sdp_repeat *r = &t->repeat[j];
			BUFFER_SNPRINTF("r=%ld %ld", r->interval, r->duration);
			for (k = 0; k < r->offsets_count; k++)
				BUFFER_SNPRINTF(" %ld", r->offsets[k]);
			BUFFER_SNPRINTF("\n");
		}
	}

	if (sdp->zone_adjustments_count) {
		BUFFER_SNPRINTF("z=");
		for (i = 0; i < sdp->zone_adjustments_count; i++)
			BUFFER_SNPRINTF("%ld %ld%s", sdp->zone_adjustments[i].adjust,
				   sdp->zone_adjustments[i].offset,
				   i + 1 < sdp->zone_adjustments_count ? " " : "");
		BUFFER_SNPRINTF("\n");
	}

	if (sdp->encrypt_key)
		BUFFER_SNPRINTF("k=%s\n", sdp->encrypt_key);

	for (i = 0; i < sdp->attributes_count; i++)
		BUFFER_SNPRINTF("a=%s\n", sdp->attributes[i]);

	for (i = 0; i < sdp->medias_count; i++) {
		struct sdp_media *m   = &sdp->medias[i];
		struct sdp_info *info = &m->info;

		BUFFER_SNPRINTF("m=%s %d", info->type, info->port);
		if (info->port_n)
			BUFFER_SNPRINTF("/%d", info->port_n);
		BUFFER_SNPRINTF(" %s", info->proto);
		for (j = 0; j < info->fmt_count; j++)
			BUFFER_SNPRINTF(" %d", info->fmt[j]);
		BUFFER_SNPRINTF("\n");

		if (m->title)		 BUFFER_SNPRINTF("i=%s\n", m->title);
		if (m->conn.nettype && m->conn.addrtype && m->conn.address)
			BUFFER_SNPRINTF("c=%s %s %s\n",
				   m->conn.nettype, m->conn.addrtype, m->conn.address);
		for (j = 0; j < m->bw_count; j++)
			BUFFER_SNPRINTF("b=%s:%s\n", m->bw[j].bwtype, m->bw[j].bandwidth);
		if (m->encrypt_key)  BUFFER_SNPRINTF("k=%s\n", m->encrypt_key);
		for (j = 0; j < m->attributes_count; j++)
			BUFFER_SNPRINTF("a=%s\n", m->attributes[j]);
	}

	if (sdp->ssrc)
	{
		BUFFER_SNPRINTF("y=%s\n",sdp->ssrc);
	}

	if (sdp->vformat.codec_format != 0 ||
		sdp->aformat.codec_format != 0)
	{
		BUFFER_SNPRINTF("f=v");

		if (sdp->vformat.codec_format == 0)
		{
			BUFFER_SNPRINTF("/////");
		}
		else
		{
			BUFFER_SNPRINTF("/%d/%d/%d/%d/%d",
				sdp->vformat.codec_format,
				sdp->vformat.video_resolution,
				sdp->vformat.frame_rate,
				sdp->vformat.bit_rate,
				sdp->vformat.kps_rate);
		}

		BUFFER_SNPRINTF("a");
		if (sdp->aformat.codec_format == 0)
		{
			BUFFER_SNPRINTF("///");
		}
		else
		{
			BUFFER_SNPRINTF("/%d/%d/%d",
				sdp->aformat.codec_format,
				sdp->aformat.kps_rate,
				sdp->aformat.freq_rate);
		}
		BUFFER_SNPRINTF("\n");
	}

	return retsize;
	
}

int sdp_dump(struct sdp_payload *sdp,char**ppRetString,int *pLen)
{
	char *pRetString = *ppRetString;
	int lensize = *pLen;
	int retsize;
	int ret;

    if (!sdp) {
        ERROR("invalid SDP\n");
		errno = EINVAL;
        return -1;
    }

	if (pRetString == NULL || lensize == 0)
	{
		if (lensize == 0)
		{
			lensize = 512;
		}

		pRetString = (char*)malloc(lensize);
	}

	if (pRetString == NULL)
	{
		return -1;
	}

	do
	{
		retsize = __sdp_dump(sdp,pRetString,lensize);
		if (retsize == -1)
		{
			if (pRetString && pRetString != *ppRetString)
			{
				free(pRetString);
			}
			pRetString = NULL;

			lensize <<= 1;
			pRetString = (char*)malloc(lensize);
			if (pRetString == NULL)
			{
				return -1;
			}
		}
	}while(retsize == -1);

	if (retsize < 0)
	{
		ret = -1;
		goto fail;
	}

	*ppRetString = pRetString;
	*pLen = lensize;

	return retsize;

fail:
	if (pRetString && *ppRetString != pRetString)
	{
		free(pRetString);
	}
	pRetString = NULL;
	return ret;
}


