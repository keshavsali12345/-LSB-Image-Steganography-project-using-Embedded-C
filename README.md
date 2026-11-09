
 # LSB Image Steganography using C

## About the Project
LSB (Least Significant Bit) Image Steganography is a C programming project used to hide secret text data inside a BMP image.

It uses the LSB technique to store secret data in the image pixels. The hidden data can be retrieved later using the decoding process.

## Features
- Hide secret text files inside BMP images.
- Retrieve hidden data from stego images.
- Support both encoding and decoding.
- Preserve the original image appearance.
- Use command-line arguments to run the program.

## Technologies Used
- C Programming
- Linux
- GCC Compiler
- Makefile

## Concepts Used
- File Handling
- Pointers
- Structures
- Bitwise Operators
- Dynamic Memory Allocation
- Command-Line Arguments
- Modular Programming

## Project Structure
```text
LSB-Steganography/
│
├── main.c
├── encode.c
├── decode.c
├── encode.h
├── decode.h
├── types.h
├── common.h
├── Makefile
└── README.md
```

## Compilation
Compile the project using:

```bash
make
```

## How to Run

### Encoding
Use the following command to hide a secret text file inside a BMP image:

```bash
./a.out -e beautiful.bmp secret.txt stego.bmp
```

### Decoding
Use the following command to retrieve the hidden data:

```bash
./a.out -d stego.bmp
```

## Learning Outcomes
- Learned how to perform file operations in C.
- Improved understanding of pointers and structures.
- Practiced bitwise operations.
- Learned how to encode and decode data at the bit level.
- Gained experience in debugging and Linux-based development.

## Conclusion
This project helped me improve my C programming skills and understand how data can be hidden inside an image using bitwise operations.


