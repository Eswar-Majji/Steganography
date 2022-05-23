#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
 
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if (strcmp(strstr(argv[2],"."),".bmp") == 0)
    {
        encInfo -> src_image_fname = argv[2];
    }
    else
    {
        return e_failure;
    }
    
    if (strcmp(strstr(argv[3],"."),".txt") == 0)
    {
        encInfo -> secret_fname = argv[3];
    }
    else
    {
        return e_failure;
    }
    
    if (argv[4] != NULL)
    {
        encInfo -> stego_image_fname = argv[4];
    }
    else
    {
        encInfo -> stego_image_fname = "stego.bmp";
    }
    return e_success;
}
 
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}
uint get_file_size(FILE *fptr_secret)
{
    fseek(fptr_secret, 0, SEEK_END);
    return ftell(fptr_secret);
}

Status check_capacity(EncodeInfo *encInfo)
{
    encInfo -> image_capacity = get_image_size_for_bmp(encInfo -> fptr_src_image);
    encInfo -> size_secret_file = get_file_size(encInfo -> fptr_secret);
    if (encInfo -> image_capacity > ( 54 + ( 2 + 4 +  4 + 4 + encInfo -> size_secret_file) * 8 ))
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}
Status copy_bmp_header(FILE *fptr_src, FILE *fptr_stego)
{
    //fread - .bmp is non human readable
    fseek(fptr_src, 0, SEEK_SET);
    char str[54];
    fread(str, sizeof(char), 54, fptr_src);
    fwrite(str, sizeof(char), 54, fptr_stego);
    return e_success;
}
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    //# --- 0010 0011
    //fetch 1 bit at a time and encode in the byte of image image_buffer
    unsigned int mask = 1 << 7;
    for(int i = 0; i < 8; i++)
    {
        //data and mask ---- will give the MSB bit 
        //bring the MSB to LSB
        //(data & mask) >> 7
        // fetch first byte from image buffer[i]
        //put the 1 bit in LSB of image buffer
        image_buffer[i] = ( image_buffer[i] & 0xFE ) | ( ( data & mask ) >> ( 7 - i ) );
        mask = mask >> 1;
    }
    return e_success;
}
Status encode_size_to_lsb(char *buffer, int size)
{
    unsigned int mask = 1 << 7;
    for(int i = 0; i < 32; i++)
    {
        buffer[i] = ( buffer[i] & 0xFE ) | ( ( size & mask ) >> ( 7 - i ) );
        mask = mask >> 1;
    }
    return e_success;
}
Status encode_size(int size, FILE *src, FILE *stego)
{
    //4 bytes to encode
    char str[32];
    fread(str, 32, sizeof(char), src);
    encode_size_to_lsb(str, size);
    fwrite(str, 32, sizeof(char), stego);
    return e_success;
}
Status encode_data_to_image(const char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image, EncodeInfo *encInfo)
{
   //call encode byte to lsb
   //each time pass 1 byte along with 8 byte beautiful.bmp
   for(int i = 0; i < size; i++)
   {
       //read 8 bytes from breautifull.bmp
       fread(encInfo -> image_data, sizeof(char), 8, fptr_src_image);
       encode_byte_to_lsb(data[i], encInfo -> image_data);
       fwrite(encInfo -> image_data, sizeof(char), 8, fptr_stego_image);
   }
   return e_success;
   
}
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    //every encoding need to call encode_data_to_image
    encode_data_to_image(magic_string, strlen(magic_string), encInfo -> fptr_src_image, encInfo -> fptr_stego_image, encInfo);
    return e_success;
}
Status encode_secret_file_extn(const char *file_ext, EncodeInfo *encInfo)
{
    file_ext = ".txt";
    encode_data_to_image(file_ext, strlen(file_ext), encInfo -> fptr_src_image, encInfo -> fptr_stego_image, encInfo);
    return e_success;
}
Status encode_secret_file_size(long int size, EncodeInfo *encInfo)
{
    //4 bytes to encode
    char str[32];
    fread(str, 32, sizeof(char), encInfo -> fptr_src_image);
    encode_size_to_lsb(str, size);
    fwrite(str, 32, sizeof(char), encInfo -> fptr_stego_image);
    return e_success;
}
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char ch;
    //bring the secret file pointer to first position
    fseek(encInfo -> fptr_secret, 0, SEEK_SET);
    for (int i = 0; i < encInfo -> size_secret_file; i++)
    {
        fread(encInfo -> image_data, 8, sizeof(char), encInfo -> fptr_src_image);
        fread(&ch, 1, sizeof(char), encInfo -> fptr_secret);
        encode_byte_to_lsb(ch, encInfo -> image_data);
        fwrite(encInfo -> image_data, 8, sizeof(char), encInfo -> fptr_stego_image);
    }
    return e_success;
}
Status copy_remaning_img_data(FILE *fptr_src, FILE *fptr_stego)
{
    char ch;
    while (fread(&ch, 1, 1, fptr_src) > 0)
    {
        fwrite(&ch, 1, 1, fptr_stego);
    }
    return e_success;
}
Status do_encoding(EncodeInfo *encInfo)
{
    //call rest of the Function
    //open the files
    if (open_files(encInfo) == e_success)
    {
        printf("File Opened successfully\n");
        printf("Staretd Encoding\n");
        if (check_capacity(encInfo) == e_success)
        {
            printf("Secret Data Can be Encoded in .bmp\n");
            //copy 54 byte header
            if (copy_bmp_header(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success)
            {
                printf("Copied Header Successfully\n");
                if(encode_magic_string(MAGIC_STRING, encInfo) == e_success)
                {
                    printf("Magic string encoded successfully\n");
                    //encode secret file extension
                    if (encode_size(strlen(".txt"), encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success)
                    {
                        printf("Encoded Secret File Size Successfully\n");
                        if (encode_secret_file_extn(encInfo -> extn_secret_file, encInfo) == e_success)
                        {
                            printf("Encoded Extension Successfully\n");
                            if (encode_secret_file_size(encInfo -> size_secret_file, encInfo) == e_success)
                            {
                                printf("Encoded secret file successfully\n");
                                if (encode_secret_file_data(encInfo) == e_success)
                                {
                                    printf("Encoded Secret date Successfully\n");
                                    if(copy_remaning_img_data(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success)
                                    {
                                        printf("Copied Remaining data\n");
                                    }
                                    else
                                    {
                                        printf("Failed to Copy Remaining data\n");
                                        return e_failure;
                                    }
                                }
                                else
                                {
                                    printf("Failed to Encode the Secret data\n");
                                    return e_failure;
                                }
                            }
                            else
                            {
                                printf("Failed to Encode thr Secret File\n");
                                return e_failure;
                            }
                        }
                        else
                        {
                            printf("Failed to Encode the Extension\n");
                            return e_failure;
                        }
                    }
                    else
                    {
                        printf("Failed to Encode the Secret File Size\n");
                        return e_failure;
                        
                    }
                }
                else
                {
                    printf("Magic string didn't Encoded\n");
                    return e_failure;
                }
            }
            else
            {
                printf("Header Didn't Copied\n");
                return e_failure;
            }
        }
        else
        {
            printf("Secret Data Can't be Encodedin .bmp\n");
            return e_failure;
        }
    }
    else
    {
        printf("File Opening Failed\n");
        return e_failure;
    }
    return e_success;
}








