#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "common.h"
//..........................................Read and validate files.................
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    // Validate stego image file (.bmp)
    if (argv[2] != NULL && strcmp(strstr(argv[2], "."), ".bmp") == 0)
    {
        decInfo->stego_image_fname = argv[2];
    }
    else
    {
        fprintf(stderr, "ERROR: Invalid stego image file. Must be .bmp\n");
        return e_failure;
    }

    // Optional output file name
    if (argv[3] != NULL && strcmp(strstr(argv[3],"."),"txt") == 0)
    {
        decInfo->output_fname = argv[3];
    }
    else
    {
        decInfo->output_fname = "decoded_secret.txt"; // default
    }

    return e_success;
}
//.........................................................................
// Helper: decode one byte from 8 LSBs
static Status decode_byte_from_lsb(char *image_buffer, char *data)
{
    unsigned char ch = 0;
    for (int i = 0; i < 8; i++)
    {
        ch = (ch << 1) | (image_buffer[i] & 1);
    }
    *data = ch;
    //printf("one charecter extract__%c\n", *data);
    return e_success;
}

// Helper: decode 32-bit size from 32 LSBs
static Status decode_size_from_lsb(char *image_buffer, int *size)
{
    unsigned int value = 0;
    for (int i = 0; i < 32; i++)
    {
        value = (value << 1) | (image_buffer[i] & 1);
    }
    *size = value;
    return e_success;
}
//...............................................................................


/*
 * Function: open_decode_files
 * ---------------------------
 * Opens the stego BMP image (containing hidden data) and the output
 * file where the decoded secret will be written
 */
Status open_decode_files(DecodeInfo *decInfo)
{   //stego.bmp file encoded file...
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    if (decInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);
        return e_failure;
    }
    decInfo->fptr_output = fopen(decInfo->output_fname,"w");
    if(decInfo->fptr_output == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);
        return e_failure;
    }
    return e_success;
}
//.....................................................................................


/*
 * Function: decode_magic_string
 * -----------------------------
 * Decodes the predefined magic string from the stego BMP image
 */
Status decode_magic_string(DecodeInfo *decInfo)
{
    char buffer[8];
    char magic[strlen(MAGIC_STRING) + 1];
    fseek(decInfo->fptr_stego_image, 54, SEEK_SET); // skip BMP header

    for (int i = 0; i < strlen(MAGIC_STRING); i++)
    {
        fread(buffer, 8, sizeof(char), decInfo->fptr_stego_image);
        decode_byte_from_lsb(buffer, &magic[i]);
    }
    magic[strlen(MAGIC_STRING)] = '\0';

    if (strcmp(magic, MAGIC_STRING) == 0)
    {
        printf("Magic string verified: %s\n", magic);
        return e_success;
    }
    else
    {
        printf("Magic string mismatch!\n");
        return e_failure;
    }
    return e_success;
}

/*
 * Function: decode_secret_file_extn
 * ---------------------------------
 * Decodes the hidden secret file extension from the stego BMP image
 */
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    char buffer[32];
    int extn_size;

    fread(buffer, 32, sizeof(char), decInfo->fptr_stego_image);
    decode_size_from_lsb(buffer, &extn_size);

    for (int i = 0; i < extn_size; i++)
    {
        fread(buffer, 8, sizeof(char), decInfo->fptr_stego_image);
        decode_byte_from_lsb(buffer, &decInfo->extn_secret_file[i]);
    }
    decInfo->extn_secret_file[extn_size] = '\0';
    if(strcmp(decInfo->extn_secret_file, ".txt") == 0)
    {
       printf("Decoded extension: %s\n", decInfo->extn_secret_file);
       return e_success;
    }
    
    return e_failure;
}

// Decode secret file size
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    char buffer[32];
    fread(buffer, 32, sizeof(char), decInfo->fptr_stego_image);
    decode_size_from_lsb(buffer, &decInfo->secret_file_size);

    printf("Decoded secret file size: %d\n", decInfo->secret_file_size);
    return e_success;
}
//......................................................................................

/*Function: decode_secret_file_data
 *---------------------------------
 *decode the hidden secret file data from stego BMP image*/
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char buffer[8];
    char ch;

    decInfo->output_fname = "decoded_secret.txt"; // default output
    decInfo->fptr_output = fopen(decInfo->output_fname, "w");
    if (decInfo->fptr_output == NULL)
    {
        perror("fopen");
        return e_failure;
    }

    for (int i = 0; i < decInfo->secret_file_size; i++)
    {
        fread(buffer, 8, sizeof(char), decInfo->fptr_stego_image);
        decode_byte_from_lsb(buffer, &ch);
        fwrite(&ch, 1, 1, decInfo->fptr_output);
    }

    fclose(decInfo->fptr_output);
    printf("Decoded secret file data successfully\n");
    return e_success;
}
//................................................................................

/*
 * Function: do_decoding
 * ---------------------
 * Performs the complete decoding process to extract the hidden
 * secret file from the stego BMP image using LSB steganography.
 */
Status do_decoding(DecodeInfo *decInfo)
{
    if (open_decode_files(decInfo) == e_success)
    {
        printf("Openfile file successfully");
        if(decode_magic_string(decInfo) == e_success)
        {   
            printf("Magic string decoded successfully");

            if(decode_secret_file_extn(decInfo) == e_success)
            {
                printf("secret file extension decoded successfully...\n");

                if(decode_secret_file_size(decInfo) == e_success)
                {
                   printf("secret file size decoded successfully...\n");

                   if(decode_secret_file_data(decInfo) == e_success)
                   {
                     printf("Secret file data decoded successfully...\n");
                   }
                   else
                   {
                    printf("Secret file data not decoded successfully...\n");
                    return e_failure;
                   }
                }
                else
                {
                   printf("secret file size not decoded successfully...\n");
                   return e_failure;
                }
            }
            else
            {
                printf("secret file extension not decoded successfully...\n");
                return e_failure;
            }
        }
        else
        {   
            printf("Magic string not decoded successfully...");
            return e_failure;
        }
    }
    else
    {    printf("File not opened successfully...");
         return e_failure;
    }
    
    return e_success;
    
}
//************************************************************************************/
