#include <stdio.h>
#include "encode.h"
#include "types.h"
#include<string.h>
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
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
 * Function: open_files
 * --------------------
 * Opens source BMP, secret text, and stego BMP files needed for encodig
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
//..........................................................................

/*
 * Function: read_and_validate_encode_args
 * ---------------------------------------
 * Validates command-line arguments for encoding operation.
 */

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{   // validate .bmp file.
    if(argv[2] != NULL && strcmp(strstr(argv[2], "."), ".bmp") == 0)
    {
        encInfo->src_image_fname = argv[2];
        
    }
    else
    {
        return e_failure;
    }
    //validate .txt
    if( argv[3] != NULL && strcmp(strstr(argv[3], "."), ".txt") == 0)
    {
        //fetch and store filename in the structure
        encInfo->secret_fname = argv[3];
    }
    else
    {
        return e_failure;
    }
    // optional output image file
    if(argv[4] != NULL)
    {
        encInfo->stego_image_fname = argv[4];
    }
    else
    {
        encInfo->stego_image_fname ="stego.bmp";
    }
    return e_success;
}
//................................................................................


/*
 * Function: encode_secret_file_data
 * ---------------------------------
 * Encodes the contents of the secret text file into the source BMP image
 * using Least Significant Bit (LSB) steganography
 */
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char ch;
    fseek(encInfo->fptr_secret, 0, SEEK_SET);
    for(int i = 0; i < encInfo->size_secret_file; i++)
    {
        fread(encInfo->image_data,8,sizeof(char),encInfo->fptr_src_image);
        fread(&ch, 1 ,1, encInfo->fptr_secret);
        encode_byte_to_lsb(ch, encInfo->image_data);
        fwrite(encInfo->image_data, 8,sizeof(char),encInfo->fptr_stego_image);
    }
    return e_success;
}
//..........................................................................................
/*
 * Function: copy_remaining_img_data
 * ---------------------------------
 * Copies the remaining image data from the source BMP file to the
 * destination (stego) BMP file after encoding is complete.
 */
Status copy_remaining_img_data(FILE *fptr_scr, FILE *fptr_dest)
{
    char ch;
    while(fread(&ch, 1,1,fptr_scr) > 0)
    {
        fwrite(&ch, 1, 1,fptr_dest);
    }
    return e_success;
}
//..........................................................................................


/*
 * Function: encode_secret_file_extn
 * ---------------------------------
 * Encodes the extension of the secret file into the BMP image
 * using Least Significant Bit (LSB) steganography
 */
Status encode_secret_file_extn(const char *file_extn,EncodeInfo *encInfo)
{
    file_extn = ".txt";
    encode_data_to_image(file_extn,strlen(file_extn), encInfo->fptr_src_image, encInfo->fptr_stego_image,encInfo);
    return e_success;

}
//..........................................................................................


/*
 * Function: get_file_size
 * -----------------------
 * Calculates the size of the given file in bytes
 */
uint get_file_size(FILE *fptr_secr)
{
    fseek(fptr_secr, 0, SEEK_END);
    return ftell(fptr_secr);
}
//.............................................................................


/*
 * Function: encode_size_to_lsb
 * ----------------------------
 * Encodes an integer value (such as file size or extension length)
 * into the least significant bits of image data
 */
Status encode_size_to_lsb(int  data, char *image_buffer)
{
    unsigned int  mask = 1 << 31;
    for(int i = 0; i < 32; i++)
    {
        image_buffer[i] = (image_buffer[i] & 0xFE) | ((data & mask) >> (31 - i));
        mask = mask >> 1;
    }

    return e_success;
}
//............................................................................


/*
 * Function: encode_byte_to_lsb
 * ----------------------------
 * Encodes a single byte of data into the least significant bits
 * of 8 consecutive bytes in the image buffer
 */
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    unsigned char mask = 1 << 7;
    for(int i = 0; i < 8; i++)
    {
        image_buffer[i] = (image_buffer[i] & 0xFE) | ((data & mask) >> (7 - i));
        mask = mask >> 1;
    }

    return e_success;
}
//...............................................................


/*
 * Function: encode_data_to_image
 * ------------------------------
 * Encodes a sequence of bytes (secret data) into the BMP image
 * using Least Significant Bit (LSB) steganography
 */
Status encode_data_to_image( const char *data, int size, FILE *fptr_src,FILE *fptr_stego, EncodeInfo *encInfo )
{
    //for encode byte_ to lsb to encode the data one after one
    for(int i = 0; i < size; i++)
    {   
        //read 8 byte of rgb 
        fread(encInfo->image_data,  8 , sizeof(char), fptr_src);
       //image-data 8 byte char fetch 8 byte 
        encode_byte_to_lsb(data[i], encInfo->image_data);
        fwrite(encInfo->image_data,8,sizeof(char),fptr_stego);
    }
     return e_success;
}
//................................................................................................


/*
 * Function: encode_size
 * ---------------------
 * Encodes an integer value (e.g., secret file size) into the
 * least significant bits of 32 bytes from the BMP image
 */

Status encode_size(int size,FILE *fptr_scr, FILE *fptr_stego)
{
  char str[32]; // read 32 byte data from beautiful.bmp 
  fread(str,32,sizeof(char), fptr_scr);
  encode_size_to_lsb(size, str);
  fwrite(str,32,sizeof(char), fptr_stego);
  return e_success;
}
//............................................................................................


/*
 * Function: encode_secret_file_size
 * ---------------------------------
 * Encodes the size of the secret file into the BMP image using
 */
Status encode_secret_file_size( int size, EncodeInfo *encInfo)
{
  char str[32]; // read 32 byte data from beautiful.bmp 
  fread(str,32,sizeof(char),encInfo->fptr_src_image );
  encode_size_to_lsb(size, str);
  fwrite(str,32,sizeof(char), encInfo->fptr_stego_image);
  return e_success;
}
//.................................................................................................


/*
 * Function: encode_magic_string
 * -----------------------------
 * Encodes a predefined magic string into the BMP image to mark the
 * presence of hidden data
 */
Status encode_magic_string(const char *magic_str, EncodeInfo *encInfo)
{
      //every charecter encoding wii have to call this 
      encode_data_to_image(magic_str, strlen(magic_str), encInfo->fptr_src_image,encInfo->fptr_stego_image, encInfo);
      return e_success;
}
//.......................................................................................................

/*
 * Function: check_capacity
 * ------------------------
 * Checks whether the source BMP image has enough capacity to
 */

Status check_capacity(EncodeInfo *encInfo)
{
    //getting size of sorce image file;
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    //getting size of secret file.
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    if(encInfo->image_capacity > (54 + ((2 + 4 + 4 + encInfo->size_secret_file) * 8)))
    {
      return e_success;
    }
    else
    {
      return e_failure;
    }
}
//...................................................................................
/*
 * Function: copy_bmp_header
 * -------------------------
 * Copies the 54-byte BMP header from the source image to the
 * destination (stego) image
 */
Status copy_bmp_header(FILE *fptr_src, FILE *fptr_stego )
{
    char header[54];
    fseek(fptr_src, 0, SEEK_SET);
    
    fread(header, 54, sizeof(char), fptr_src);//reading 54 byte header from source
    fwrite(header,54, sizeof(char), fptr_stego);//writing 54 byte to stego image
    return e_success;

}
//..................................................................................................

/*
 * Function: do_encoding
 * ---------------------
 * Performs the complete encoding process to hide the secret file
 */
Status do_encoding(EncodeInfo *encInfo)
{
    //open all the file which is required to encoding
    if(open_files(encInfo) == e_success)
    {
        printf("Opened file successfully");
        printf("Started Encoding....\n");
        if(check_capacity(encInfo) == e_success)
        {
            printf("Image has enough capacity the secret message\n");
            if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)
            {
                printf("Successfully copied the headeer\n");
                if(encode_magic_string(MAGIC_STRING, encInfo) == e_success)
                {
                    printf("magic string encoded successfully\n");
                    if(encode_size(strlen(".txt"),encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)
                    {
                        printf("Successefully encode the secret file extension size\n");
                        
                        if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo) == e_success)
                        {
                            printf("Successfully encoded the secret file extension\n");
                            if(encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success)
                            {
                                printf("Successfully encode  the secret file size\n");
                            }
                            else
                            {
                              printf("failed to encode secret file size\n");
                              return e_failure;
                            }
                            if(encode_secret_file_data(encInfo) == e_success)
                            {
                                printf("Encoded secret data successfully\n");
                                if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)
                                {
                                    printf("Copied remaaining RBG data successfully\n");
                                }
                                else
                                {
                                    printf("Failed to copy remaining data\n");
                                    return e_failure;
                                }
                            }
                            else
                            {
                                printf("Failed to encode the secret file data");
                                return e_failure;
                            }
                        }
                        else
                        {
                            printf("failed to encode the secret file extension\n");
                            return e_failure;
                        }
        
                        
                    }
                    else
                    {
                        printf("failed to encode the ");
                        return e_failure;
                    }
                }
                else
                {
                    printf("Failed to encode the magic string");
                    return e_failure;
                }
            }
            else
            {
                printf("Failed to copy the header\n");
                return e_failure;
            }

        }
        else
        {
           printf("Dont have enough capacity the secret message\n");
           return e_failure;
        }
    }
    else
    {
        printf("Failed to open the files\n");
        return e_failure;
    } 
   
    return e_success;
   
}