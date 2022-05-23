#ifndef DECODE_H
#define DECODE_H

#include "types.h" //contains user defined types
#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

static char dec_mag_str[MAX_SECRET_BUF_SIZE];
static char dec_file_ext[MAX_IMAGE_BUF_SIZE];
static char decoded_data;
int op_file_mention;


/* struct is defined for stego.bmp file information and secret.txt file */

typedef struct _DecodeInfo
{
    /* stego.bmp file information */
       char *stego_image_fname;
       FILE *fptr_stego_image;
       char image_data[MAX_IMAGE_BUF_SIZE];


    /* decoded file information */
       char *decoded_fname;
       FILE *fptr_decoded;
       char *decoded_size;
       char *magic_data;
       char dec_mag_str[MAX_SECRET_BUF_SIZE];
       char dec_mag_data[MAX_SECRET_BUF_SIZE];

       //seccret file information
       long dec_file_size[MAX_SECRET_BUF_SIZE];
       char dec_file_data[MAX_SECRET_BUF_SIZE]; 
       char *dec_data;
} DecodeInfo;

/*Function prototypes*/

//to open files
Status Open_files(DecodeInfo *decInfo);

/* raed and validate args */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* do decode */
Status do_decoding(DecodeInfo *decInfo);

/* decode magic string */
Status decode_magic_string_and_cmp(char *magic_string, DecodeInfo *decInfo);

//decode magic string data
Status decode_magic_string_data(const char *magic_string, long int size, FILE *fptr_stego_image, DecodeInfo *decInfo);

/* decode_byte_from_lsb */
Status decode_byte_from_lsb(char *image_data, char *dec_image_str, long int position, DecodeInfo *decInfo);


//decode the size of the .txt file
Status decode_file_size(char *size, DecodeInfo *decInfo);
//decode file extension
Status decode_byte_from_lsb_ext(char *ext_data, char *dec_file_ext, long int position, DecodeInfo *decInfo);

//decode file data from lsb
Status decode_file_data_fromlsb(char *data, char *dec_data, DecodeInfo *decInfo);
//decode file data
Status decode_secret_data(DecodeInfo *decInfo);
#endif
