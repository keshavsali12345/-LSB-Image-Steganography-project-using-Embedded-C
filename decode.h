#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include "types.h"

// Structure for decoding info
typedef struct _DecodeInfo
{
    char *stego_image_fname;   // input stego image
    FILE *fptr_stego_image;

    char *output_fname;        // output secret file
    FILE *fptr_output;

    int secret_file_size;      // size of hidden file
    char extn_secret_file[10]; // extension of hidden file
} DecodeInfo;

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);
// Function prototypes
Status open_decode_files(DecodeInfo *decInfo);

Status decode_magic_string(DecodeInfo *decInfo);

Status decode_secret_file_extn(DecodeInfo *decInfo);

Status decode_secret_file_size(DecodeInfo *decInfo);

Status decode_secret_file_data(DecodeInfo *decInfo);

Status do_decoding(DecodeInfo *decInfo);

#endif
