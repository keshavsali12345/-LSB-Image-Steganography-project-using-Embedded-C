/***************************************************************************
 * Project: Stegnography (Encoding & Decoding)
 * File   : mainc.c
 * Author : Keshav
 *
 * Description:
 * This project implements LSB Bit stegnography.
 * It allows encoding secret text data into a .bmp image file & 
 * decoding the hidden data back from the stego image.

 * Features:
 *  -- Encoading: Hide secret text inside BMP image pixels.
 *  -- Decoading: Extract hidden data from .bmp image.
 * Command for encode & and decode.
 *
 * Encoding: ./a.out -e beautiful.bmp secret.tex stego.bmp
 * Decoding: ./a.out -d stego.bmp\n");


***************************************************************************/
#include <stdio.h>
#include "encode.h"
#include "types.h"
#include<string.h>
#include "decode.h"

/*This Main function collects arguments form CLA based on that will perform encoding or decoding*/
int main(int argc, char *argv[])
{   

    //check operation type to know whther it id encodeing or decoding
    if(check_operation_type(argv) == e_encode)
    {   
        EncodeInfo encInfo; // Stucture decleration
        
        printf("Selected Encoding\n");
        if(read_and_validate_encode_args(argv, &encInfo) == e_success)  
        {
            printf("Read and validate encode arguments is successful\n");
            if(do_encoding(&encInfo) == e_success)
            {
                printf("Encoding completed...\n");
            }
        }
        else
        {
             printf("Read and validate encode arguments is failed\n");
        }

    }
    else if(check_operation_type(argv) == e_decode)
    {    
        DecodeInfo decInfo; // decode stucture defination
        printf("Selected decoding\n");
        if(read_and_validate_decode_args(argv, &decInfo) == e_success)
        {    
             printf("Read and validate decode arguments is successful\n");
             if(do_decoding(&decInfo) == e_success)  
             {
                printf("Decoding Completed...\n");
             }
        }
        else
        {
             printf("Read and validate encode arguments is failed\n");
        }
        
    }
    else
    {
        printf("Invalid Option\n********************************Uase**********************\n");
        printf("Encoding: ./a.out -e beautiful.bmp secret.tex stego.bmp\n");
        printf("Decoding: ./a.out -d stego.bmp\n");
        printf("**************************************");
    }
    return 0;
}


/* This function determine whether  perform ecode and decode based on CLA*/
OperationType check_operation_type(char *argv[])
{
    if(strcmp(argv[1], "-e") == 0)   // -e encode
    {
        return e_encode;   
    }
    else if(strcmp(argv[1], "-d") == 0) // -d decode
    {
      return e_decode;  // return 1
    }
    else
    {
        return e_unsupported;
    }
}
//...........................................................................

   
    

