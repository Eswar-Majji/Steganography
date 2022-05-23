#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

/* Function Definition */

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if (strcmp(strstr(argv[2],"."),".bmp") == 0)
    {
        decInfo -> stego_image_fname = argv[2];
    }
    else
    {
        return e_failure;
    }

    if (argv[3] != NULL)
    {
        decInfo -> decoded_fname = argv[3];
    }
    else
    {
        decInfo -> decoded_fname = "decoded.txt";
    }
    return e_success;
}

/*files opening*/
Status Open_files(DecodeInfo *decInfo)
{
    /*stego file*/
    decInfo -> fptr_stego_image = fopen(decInfo -> stego_image_fname, "r");
    //Do error handling
    if (decInfo -> fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "Error: unable to open the file %s \n", decInfo -> stego_image_fname);
        return e_failure;
    }

    //decoded file
    decInfo -> fptr_decoded = fopen(decInfo -> decoded_fname, "w");
    //do error
    if (decInfo -> fptr_decoded == NULL)
    {
        perror("fopen");
        fprintf(stderr, "Error: unable to open the file %s \n", decInfo -> decoded_fname);
        return e_failure;
    }

    //no failure
    return e_success;
}
                        /*magic string decoding functions*/

Status decode_byte_from_lsb (char *image_data, char *dec_mag_str, long int position, DecodeInfo *decInfo)
{
    int mask = 0x1;
    *dec_mag_str = 0;
    for (int i = 0; i < 8; i++)
    {
       *dec_mag_str = *dec_mag_str | ( ( image_data[i] & mask ) << ( 7 - i ) );
    }
    decInfo -> dec_mag_data[(char)position] = *dec_mag_str;
   //to clear string
    for (int i = 0; i < 8; i++)
    {
        *dec_mag_str = ( dec_mag_str[i] & 0x0 );
    }
    return e_success;
}

Status decode_magic_string_data(const char *magic_string, long int size, FILE *fptr_stego_image, DecodeInfo *decInfo)
{    
    fseek(decInfo -> fptr_stego_image, 54, SEEK_SET);
    for (int i = 0; i < 2; i++)
    {
        fread(decInfo -> image_data, sizeof(char), 8, fptr_stego_image);
        decode_byte_from_lsb(decInfo -> image_data, decInfo -> dec_mag_str, i, decInfo);
    }
    return e_success;
}

Status decode_magic_string_and_cmp(char *magic_string, DecodeInfo *decInfo)
{
    int i;
    decode_magic_string_data(magic_string, strlen(magic_string), decInfo -> fptr_stego_image, decInfo);    
    printf("Comapring Magic string\n");
    if (strncmp(decInfo -> dec_mag_data, "#*", strlen(magic_string)) == 0)
    {
        return e_success;
    }
}
                            /*Data and size decoding functions*/

Status decode_size_from_lsb(char *dec_file_size_data, long *dec_file_size, DecodeInfo *decInfo)
{
    int mask = 0x1;
    *dec_file_size = 0;
    for(int i = 0; i < 32; i++)
    {
        *dec_file_size = *dec_file_size | ( ( dec_file_size_data[i] & mask ) << ( 7 - i ) );
    }

    return e_success;
}
Status decode_data_size(DecodeInfo *decInfo)
{
    //32 bytes to decode
    char dec_file_size_data[32];
    fread(dec_file_size_data, 32, sizeof(char), decInfo -> fptr_stego_image);
    decode_size_from_lsb(dec_file_size_data, decInfo -> dec_file_size, decInfo);
    return e_success;
}
Status decode_file_data_fromlsb(char *data, char *dec_data, DecodeInfo *decInfo)
{
    int mask = 0x1;
    *dec_data = 0;
    for (int i = 0; i < 8; i++)
    {
        *dec_data = *dec_data | ( ( data[i] & mask ) << ( 7 - i ) );
    }
    decoded_data = *dec_data;
    return e_success;
}

Status decode_secret_data(DecodeInfo *decInfo)
{
    int i, size;
    size = decInfo -> dec_file_size[0];
    char data[8];
    for (i = 0; i < size; i++)
    {
        fread(data, sizeof(char), 8, decInfo -> fptr_stego_image);
        decode_file_data_fromlsb(data, decInfo -> dec_data, decInfo);
        fwrite(&decoded_data, sizeof(char), 1, decInfo -> fptr_decoded);
    }
    return e_success;
}
Status do_decoding(DecodeInfo *decInfo)
{
    printf("Required Files Opening\n");
    if(Open_files(decInfo) == e_success)
    {
        printf("Required files opened successfully\n");
        /*deoding magic string and check*/
        printf("Decoding Magic String\n");
        if (decode_magic_string_and_cmp(MAGIC_STRING, decInfo) == e_success)
        {
            //decose the file ext size
            printf("Decoded Magic String and compared\n");
            
            //skipping the secret file extension size and extension
            //moving pointer to ( 4 + 4 )
            fseek(decInfo -> fptr_stego_image, (54 + ( 2 + 4 + 4 ) * 8), SEEK_SET);
            
            printf("Decoded file is Opened successfully\n");
            //decoding size of secret data
            if (decode_data_size(decInfo) == e_success)
            {
                printf("Decoded Secret data size\n");
                //decoding secret data 
                if (decode_secret_data(decInfo) == e_success)
                {
                    printf("Decoded the Secret data\n");
                    return e_success;
                }
                else
                {
                    printf("Failed to Decode the Data\n");
                }
            }
                else
            {
                printf("Failed to Decode the Secret data size\n");
            }
        }
    
        else
        {
            printf("Decoding and comparing of magic string is failed\n");
        }
    }
    else
    {
        printf("Failed to open files\n");
    }

}
