/*Name          : Eswar Majji
  Date          : 13/05/2022
  Description   : Project - 1 -> Steganography
  Input       1 : ./a.out -e .bmp .txt
  Output      1 : <----Selected Encoding---->
                  Read and Validate is successfull
                  File Opened successfully
                  Staretd Encoding
                  width = 1024
                  height = 768
                  Secret Data Can be Encoded in .bmp
                  Copied Header Successfully
                  Magic string encoded successfully
                  Encoded Secret File Size Successfully
                  Encoded Extension Successfully
                  Encoded secret file successfully
                  Encoded Secret date Successfully
                  Copied Remaining data
                  Success : Encoding is Completed
  Input  2      : ./a.out -d stego.bmp
  Output 2      : <----Selected Decoding---->
                  Read and validate is successfull
                  Required Files Opening
                  Required files opened successfully
                  Decoding Magic String
                  Comapring Magic string
                  Decoded Magic String and compared
                  Decoded file is Opened successfully
                  Decoded Secret data size
                  Decoded the Secret data
                  Success : Decoding is Completed
 
 */
#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include "decode.h"
int main(int argc, char *argv[])
{

   //check operation type - -e 0r -d
   if (check_operation_type(argv) == e_encode)
   {
       EncodeInfo encInfo;
       printf("<----Selected Encoding---->\n");
       
       //read and validate command line argument
       if (read_and_validate_encode_args(argv, &encInfo) == e_success)
       {
           printf("Read and Validate is successfull\n");
           if (do_encoding(&encInfo) == e_success)
           {
               printf("Success : Encoding is Completed\n");
           }
           else
           {
               printf("Failure : Encoding is not Complegted\n");
           }
       }
       else
       {
           printf("Read and Validate is unsuccessfull\n");
       }
       
   }
   else if (check_operation_type(argv) == e_decode)
   {
       DecodeInfo decInfo;
       printf("<----Selected Decoding---->\n");

       //read and validate command line arguments
       if (read_and_validate_decode_args(argv, &decInfo) == e_success)
       {
           printf("Read and validate is successfull\n");
           if (do_decoding(&decInfo) == e_success)
           {
               printf("Success : Decoding is Completed\n");
           }
           else
           {
               printf("Failure : Decoding is not Completed\n");
           }
       }
       else
       {
           printf("Read and validate is unsuccessfull\n");
       }

   }
   else
   {
       printf("Invalid Option\n");
       printf("*****Usage*****\n");
       printf("Encoding: ./a.out -e beautiful.bmp secret.txt stego.bmp\n");
       printf("Decoding: ./a.out -d stego.bmp\n");
   }
    return 0;
}

//checking whether do encoding or decoding
OperationType check_operation_type(char *argv[])
{
    if (strcmp(argv[1],"-e") == 0)
    {
        return e_encode;
    }
    else if (strcmp(argv[1],"-d") == 0)
    {
        return e_decode;
    }
    else
    {
        return e_unsupported;
    }
}
 
