#ifndef TYPES_H
#define TYPES_H

/* User defined types */
typedef unsigned int uint;

/* Status will be used in fn. return type */
typedef enum
{
    e_success,
    e_failure
} Status;
//this is user define data type use for return operation type
typedef enum
{
    e_encode,
    e_decode,
    e_unsupported
} OperationType;

#endif
