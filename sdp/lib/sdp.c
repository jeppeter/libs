#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <stdarg.h>
#include "sdp.h"

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

	p->alloc_size = strlen(payload)+1;
	p->len_size = strlen(payload)+1;

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
    size_t i, j, k;
	char pRetString = *ppRetString;
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


/*this is get the offset of the pointer*/
#define COPY_SDP_PTR(dst,src,elm) \
do\
{\
	if (src->elm >= src->_payload && src->elm <= (src->_payload + src->len_size))\
	{\
		dst->elm = (src->elm - src->_payload) + dst->_payload;\
	}\
	else\
	{\
		assert(src->elm == NULL);\
	}\
}while(0)




#define COPY_SDP_STRAIGHT(dst,src,elm) \
do\
{\
	dst->elm = src->elm;\
}while(0)

#define COPY_PTR_OFFSET(dst,src,dstload,srcload,elm)   \
do\
{\
	if (src->elm >= srcload->_payload && src->elm <= (srcload->_payload + srcload->len_size))\
	{\
		dst->elm = (src->elm - srcload->_payload) + dstload->_payload;\
	}\
	else\
	{\
		assert(src->elm == NULL);\
	}\
}while(0)



static int __Copy_Sdp_Origin(struct sdp_payload* pDst,struct sdp_payload* pSrc)
{
	struct sdp_origin *pSrcOrigin,*pDstOrigin;
	pSrcOrigin = &(pSrc->origin);
	pDstOrigin = &(pDst->origin);

	COPY_PTR_OFFSET(pDstOrigin,pSrcOrigin,pDst,pSrc,username);
	COPY_SDP_STRAIGHT(pDstOrigin,pSrcOrigin,sess_id);
	COPY_SDP_STRAIGHT(pDstOrigin,pSrcOrigin,sess_version);
	COPY_PTR_OFFSET(pDstOrigin,pSrcOrigin,pDst,pSrc,nettype);
	COPY_PTR_OFFSET(pDstOrigin,pSrcOrigin,pDst,pSrc,addrtype);
	COPY_PTR_OFFSET(pDstOrigin,pSrcOrigin,pDst,pSrc,addr);
	
	return 0;
}


static int __Copy_Sdp_Emails(struct sdp_payload* pDst,struct sdp_payload* pSrc)
{
	int i,allocsize;

	if (pSrc->emails_count > 0)
	{
		allocsize = sizeof(pSrc->emails[0]) * pSrc->emails_count;
		pDst->emails = (char*) calloc(allocsize);
		if (pDst->emails == NULL)
		{
			return -ENOMEM;
		}
		COPY_SDP_STRAIGHT(pDst,pSrc,emails_count);

		for (i=0;i<pSrc->emails_count;i++)
		{
			COPY_SDP_PTR(pDst,pSrc,emails[i]);
		}
	}

	return 0;
}

static int __Copy_Sdp_Phones(struct sdp_payload* pDst,struct sdp_payload* pSrc)
{
	int i,allocsize;

	if (pSrc->phones_count > 0)
	{
		allocsize = sizeof(pSrc->phones[0]) * pSrc->phones_count;
		pDst->phones = (char*) calloc(allocsize);
		if (pDst->phones == NULL)
		{
			return -ENOMEM;
		}
		COPY_SDP_STRAIGHT(pDst,pSrc,phones_count);

		for (i=0;i<pSrc->phones_count;i++)
		{
			COPY_SDP_PTR(pDst,pSrc,phones[i]);
		}
	}

	return 0;
}

static int __Copy_Sdp_Connections(struct sdp_payload* pDst,struct sdp_payload *pSrc,struct sdp_connection *pDstConn,struct sdp_connection* pSrcConn)
{
	COPY_PTR_OFFSET(pDstConn,pSrcConn,pDst,pSrc,nettype);
	COPY_PTR_OFFSET(pDstConn,pSrcConn,pDst,pSrc,addrtype);
	COPY_PTR_OFFSET(pDstConn,pSrcConn,pDst,pSrc,address);
	return 0;
}

static int __Copy_Sdp_Bandwidth_Array(struct sdp_payload* pDst,struct sdp_payload *pSrc)
{
	struct sdp_bandwidth *pDstBw,*pSrcBw;
	int allocsize,i;

	if (pSrc->bw_count > 0)
	{
		allocsize = pSrc->bw_count * sizeof(pSrc->bw[0]);
		pDst->bw = (struct sdp_bandwidth*) calloc(allocsize);
		if (pDst->bw == NULL)
		{
			return -ENOMEM;
		}
		COPY_SDP_STRAIGHT(pDst,pSrc,bw_count);
		for (i=0;i<pSrc->bw_count;i++)
		{
			COPY_SDP_PTR(pDst,pSrc,bw[i]);
		}
	}
	return 0;
}

/*************************************
we may allocate some memory ,but we do not 
free them here ,because we free the memory in the sdp_destroy
*************************************/
static int __Copy_Sdp_Times(struct sdp_payload* pSrc,struct sdp_payload *pDst)
{
	struct sdp_time* pSrcTime,*pDstTime;
	int allocsize=0,i,j,k;
	
	if (pSrc->times_count == 0)
	{
		return 0;
	}

	allocsize = pSrc->times_count * sizeof(pSrc->times[0]);
	pDst->times = (struct sdp_time*)calloc(allocsize);
	if (pDst->times == NULL)
	{
		return -ENOMEM;
	}
	COPY_SDP_STRAIGHT(pDst,pSrc,times_count);

	for (i=0;i<pSrc->times_count;i++)
	{		
		struct sdp_time *pCurDstTime,*pCurSrcTime;
		pCurDstTime = &(pDst->times[i]);
		pCurSrcTime = &(pSrc->times[i]);
		COPY_SDP_STRAIGHT(pCurDstTime,pCurSrcTime,start_time);
		COPY_SDP_STRAIGHT(pCurDstTime,pCurSrcTime,stop_time);
		if (pCurSrcTime->repeat_count > 0)
		{
			allocsize = pCurSrcTime->repeat_count * sizeof(pCurSrcTime->repeat[0]);
			pCurDstTime->repeat = (struct sdp_repeat*)calloc(allocsize);
			if (pCurDstTime->repeat == NULL)
			{
				return -ENOMEM;
			}

			COPY_SDP_STRAIGHT(pCurDstTime,pCurSrcTime,repeat_count);
			for (j=0;j<pCurSrcTime->repeat_count;j++)
			{
				struct sdp_repeat *pCurDstRepeat,*pCurSrcRepeat;
				pCurDstRepeat = &(pCurDstTime->repeat[j]);
				pCurSrcRepeat = &(pCurSrcTime->repeat[j]);
				COPY_SDP_STRAIGHT(pCurDstRepeat,pCurSrcRepeat,interval);
				COPY_SDP_STRAIGHT(pCurDstRepeat,pCurSrcRepeat,duration);
				if (pCurSrcRepeat->offsets_count > 0)
				{
					allocsize = pCurSrcRepeat->offsets_count * sizeof(time_t);
					pCurDstRepeat->offsets = (time_t*)calloc(allocsize);
					if (pCurDstRepeat->offsets == NULL)
					{
						return -ENOMEM;
					}
					COPY_SDP_STRAIGHT(pCurDstRepeat,pCurSrcRepeat,offsets_count);
					for (k=0;k<pCurSrcRepeat->offsets_count;k++)
					{
						COPY_SDP_STRAIGHT(pCurDstRepeat,pCurSrcRepeat,offsets[k]);
					}
				}
			}
		}
	}

	return 0;

}
static int __Copy_Sdp_Adjust_Time(struct sdp_payload *pSrc,struct sdp_payload *pDst)
{
	struct sdp_zone_adjustments *pCurSrcZone,*pCurDstZone;
	int i;
	int allocsize;

	if (pSrc->zone_adjustments_count > 0)
	{
		allocsize = pSrc->zone_adjustments_count * sizeof(pSrc->zone_adjustments[0]);
		pDst->zone_adjustments = (struct sdp_zone_adjustments*) calloc(allocsize);
		if (pDst->zone_adjustments == NULL)
		{
			return -ENOMEM;
		}

		COPY_SDP_STRAIGHT(pDst,pSrc,zone_adjustments_count);
		for (i=0;i<pSrc->zone_adjustments_count;i++)
		{
			pCurSrcZone = &(pSrc->zone_adjustments[i]);
			pCurDstZone = &(pDst->zone_adjustments[i]);

			COPY_SDP_STRAIGHT(pCurDstZone,pCurSrcZone,adjust);
			COPY_SDP_STRAIGHT(pCurDstZone,pCurSrcZone,offset);
		}
	}
	return 0;	
}

static int __Copy_Sdp_Attributes_Array(struct sdp_payload *pSrc,struct sdp_payload *pDst)
{
	int i;
	int allocsize;

	if (pSrc->attributes_count > 0)
	{
		allocsize = pSrc->attributes_count * sizeof(pSrc->attributes[0]);
		pDst->attributes = (char**)calloc(allocsize);
		if (pDst->attributes == NULL)
		{
			return -ENOMEM;
		}

		COPY_SDP_STRAIGHT(pDst,pSrc,attributes_count);
		for (i=0;i<pSrc->attributes_count;i++)
		{
			COPY_SDP_PTR(pDst,pSrc,attributes[i]);
		}
	}

	return 0;
}


static int __Copy_Sdp_Media_Array(struct sdp_payload *pSrc,struct sdp_payload *pDst)
{
	int i,j,k,allocsize,ret;
	if (pSrc->medias_count > 0)	
	{
		allocsize = pSrc->medias_count * sizeof(pSrc->medias[0]);
		pDst->medias = (struct sdp_media*)calloc(allocsize);
		if (pDst->medias == NULL)
		{
			return -ENOMEM;
		}
		COPY_SDP_STRAIGHT(pDst,pSrc,medias_count);
		for (i=0;i<pSrc->medias_count;i++)
		{
			struct sdp_media *pCurSrcMedia,*pCurDstMedia;
			struct sdp_info *pCurSrcInfo,*pCurDstInfo;
			struct sdp_connection *pCurSrcConn,*pCurDstConn;
			pCurSrcMedia = &(pSrc->medias[i]);
			pCurDstMedia = &(pDst->medias[i]);
			pCurSrcInfo = &(pCurSrcMedia->info);
			pCurDstInfo = &(pCurDstMedia->info);

			/*change the type info*/
			COPY_PTR_OFFSET(pCurDstInfo,pCurSrcInfo,pDst,pSrc,type);
			COPY_SDP_STRAIGHT(pCurDstInfo,pCurSrcInfo,port);
			COPY_SDP_STRAIGHT(pCurDstInfo,pCurSrcInfo,port_n);
			COPY_PTR_OFFSET(pCurDstInfo,pCurSrcInfo,pDst,pSrc,proto);
			if (pCurSrcInfo->fmt_count > 0)
			{
				allocsize = pCurSrcInfo->fmt_count * sizeof(int);
				pCurDstInfo->fmt = (int*)calloc(allocsize);
				if (pCurDstInfo->fmt == NULL)
				{
					return -ENOMEM;
				}

				COPY_SDP_STRAIGHT(pCurDstInfo,pCurSrcInfo,fmt_count);
				for (j=0;j<pCurSrcInfo->fmt_count;j++)
				{
					COPY_SDP_STRAIGHT(pCurDstInfo,pCurSrcInfo,fmt[j]);
				}
			}

			/*now for the title*/
			COPY_PTR_OFFSET(pCurDstMedia,pCurSrcMedia,pDst,pSrc,title);			
			pCurSrcConn = &(pCurSrcMedia->conn);
			pCurDstConn = &(pCurDstMedia->conn);
			ret = __Copy_Sdp_Connections(pDst,pSrc,pCurDstConn,pCurSrcConn);
			if (ret < 0)
			{
				return ret;
			}

			if (pCurSrcMedia->bw_count > 0)
			{
				allocsize = pCurSrcMedia->bw_count * sizeof(struct sdp_bandwidth);
				pCurDstMedia->bw = (struct sdp_bandwidth*)calloc(allocsize);
				if (pCurDstMedia->bw == NULL)
				{
					return -ENOMEM;
				}
				COPY_SDP_STRAIGHT(pCurDstMedia,pCurSrcMedia,bw_count);
				for (j=0;j<pCurSrcMedia->bw_count;j++)
				{
					struct sdp_bandwidth *pCurSrcBw,*pCurDstBw;
					pCurSrcBw = &(pCurSrcMedia->bw[j]);
					pCurDstBw = &(pCurDstMedia->bw[j]);
					COPY_PTR_OFFSET(pCurDstBw,pCurSrcBw,pDst,pSrc,bandwidth);
					COPY_PTR_OFFSET(pCurDstBw,pCurSrcBw,pDst,pSrc,bwtype);
				}
			}

			COPY_PTR_OFFSET(pCurDstMedia,pCurSrcMedia,pDst,pSrc,encrypt_key);

			if (pCurSrcMedia->attributes_count > 0)
			{
				allocsize = pCurSrcMedia->attributes_count * sizeof(pCurSrcMedia->attributes[0]);
				pCurDstMedia->attributes = (char**)malloc(allocsize);
				if (pCurDstMedia->attributes == NULL)
				{
					return -ENOMEM;
				}
				COPY_SDP_STRAIGHT(pCurDstMedia,pCurSrcMedia,attributes_count);
				for (j=0;j<pCurSrcMedia->attributes_count;j++)
				{
					COPY_PTR_OFFSET(pCurDstMedia,pCurSrcMedia,pDst,pSrc,attributes[j]);
				}
			}			
		}
	}
	return 0;
}


static int __Copy_Sdp_Vformat(struct sdp_payload* pDst,struct sdp_payload* pSrc)
{
	struct sdp_video_format *pDstVformat,*pSrcVformat;

	pDstVformat = &(pDst->vformat);
	pSrcVformat = &(pSrc->vformat);

	COPY_SDP_STRAIGHT(pDstVformat,pSrcVformat,codec_format);
	COPY_SDP_STRAIGHT(pDstVformat,pSrcVformat,video_resolution);
	COPY_SDP_STRAIGHT(pDstVformat,pSrcVformat,frame_rate);
	COPY_SDP_STRAIGHT(pDstVformat,pSrcVformat,bit_rate);
	COPY_SDP_STRAIGHT(pDstVformat,pSrcVformat,kps_rate);

	return 0;
}
static int __Copy_Sdp_Aformat(struct sdp_payload* pDst,struct sdp_payload* pSrc)
{
	struct sdp_audio_format *pDstAformat,*pSrcAformat;

	pDstAformat = &(pDst->aformat);
	pSrcAformat = &(pSrc->aformat);

	COPY_SDP_STRAIGHT(pDstAformat,pSrcAformat,codec_format);
	COPY_SDP_STRAIGHT(pDstAformat,pSrcAformat,kps_rate);
	COPY_SDP_STRAIGHT(pDstAformat,pSrcAformat,freq_rate);

	return 0;
}

static int __Copy_Sdp_Extend(struct sdp_payload *pSrc,struct sdp_payload **ppDst,int extendlen)
{
	struct sdp_payload *pDst=NULL;
	int totallen=0;
	int ret;

	totallen = pSrc->alloc_size + extendlen;
	pDst = (struct sdp_payload*) calloc(sizeof(*pDst));
	if (pDst == NULL)
	{
		ret = -ENOMEM;
		goto fail;
	}

	pDst->alloc_size = totallen;
	pDst->_payload = (char*) calloc(pDst->alloc_size);
	if (pDst->_payload == NULL)
	{
		ret = -ENOMEM;
		goto fail;
	}

	pDst->len_size = pSrc->len_size;
	/*first to copy for the pay load*/
	memcpy(pDst->_payload,pSrc->_payload,pDst->len_size);

	COPY_SDP_STRAIGHT(pDst,pSrc,proto_version);
	ret = __Copy_Sdp_Origin(pDst,pSrc);
	if (ret < 0)
	{
		goto fail;
	}
	
	COPY_SDP_PTR(pDst,pSrc,session_name);
	COPY_SDP_PTR(pDst,pSrc,information);
	COPY_SDP_PTR(pDst,pSrc,uri);

	ret = __Copy_Sdp_Emails(pDst,pSrc);
	if (ret < 0)
	{
		goto fail;
	}

	ret = __Copy_Sdp_Phones(pDst,pSrc);
	if (ret < 0)
	{
		goto fail;
	}

	ret = __Copy_Sdp_Connections(pDst,pSrc,&(pDst->conn),&(pSrc->conn));
	if (ret < 0)
	{
		goto fail;
	}

	ret = __Copy_Sdp_Bandwidth_Array(pDst,pSrc);
	if (ret < 0)
	{
		goto fail;
	}

	ret = __Copy_Sdp_Times(pDst,pSrc);
	if (ret < 0)
	{
		goto fail;
	}

	ret = __Copy_Sdp_Adjust_Time(pSrc,pDst);
	if (ret < 0)
	{
		goto fail;
	}

	COPY_SDP_PTR(pDst,pSrc,encrypt_key);

	ret = __Copy_Sdp_Attributes_Array(pSrc,pDst);
	if (ret < 0)
	{
		goto fail;
	}

	ret = __Copy_Sdp_Media_Array(pSrc,pDst);
	if (ret < 0)
	{
		goto fail;
	}

	COPY_SDP_PTR(pDst,pSrc,ssrc);

	ret = __Copy_Sdp_Vformat(pDst,pSrc);
	if (ret < 0)
	{
		goto fail;
	}

	ret = __Copy_Sdp_Aformat(pDst,pSrc);
	if (ret < 0)
	{
		goto fail;
	}

	*ppDst = pDst;


	return 0;
fail:
	if (pDst)
	{
		sdp_destroy(pDst);
	}
	pDst = NULL;
	return ret;
}


/*
   notice the pConfig is in the pSdp->_payload
*/
int __Sdp_Parse_Config(struct sdp_payload* pSdp,char* pConfig)
{
}

/*********************************************************
*
* @function :
*          sdp_insert to insert the configuration into the sdp_payload structure
*
* @params:
*          pSrc   the original sdp_payload to insert if it is NULL ,it will allocate a new in the ppDst
*          ppDst  the return dest of sdp structure if necessary
*          pConfig the parameters to insert it will like the format
*                    a=...
*                    v=...
*                    c=...
*          it can be format for the config format
*
* @return value :
*          1 for has modify ppDst and success
*          0 for not has modify ppDst and no allocate memory for this and success
*          negative value ,error code
*********************************************************/
int sdp_insert(struct sdp_payload* pSrc,struct sdp_payload** ppDst,const char* pConfig,...)
{
	struct sdp_payload* pDst=NULL;
	int ret;
	int payload_size=512,payload_len;
	

	/*first to determine whether the pConfig is the right format of the sdp line*/
	pDst = (struct sdp_payload*)calloc(sizeof(*pDst));
	if (pDst == NULL)
	{
		ret = -ENOMEM;
		goto fail;
	}

	payload_len = strlen(pConfig);
	

	/*now to parse the value*/
	

	ret = 0;
	if (pDst)
	{
		ret = 1;
		*ppDst = pDst;
	}
	return ret;
fail:
	if (pDst)
	{
		sdp_destroy(pDst);
	}
	assert(ret < 0);
	return ret;
}

