/**************************************************************
* Class:  CSC-415-03 Fall 2022
* Name: Abbas Mahdavi
* Student ID: 918345420
* GitHub UserID: AbbasMahdavi021
* Project: Assignment 6 – Device Driver
*
* Description: Implements device driver open, read, write, 
*              release and icotl function as well as 
*              starting and ending the driver.
**************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#define BUFFER_LENGTH 1024
#define READVALUE _IOR('a', 'b', int)
#define WRITEVALUE _IOW('a', 'a', int)

static char getNum[BUFFER_LENGTH];

//Simple test that takes in an number,
//write it to the device driver, and reads the number back!
int main() {

    //init variables
    int returnedValue, fileDirectory;
    char writtenNumber[BUFFER_LENGTH];
    int value, number;
    printf("Device Driver Test Started!\n");

    //Open the device driver
    printf("\nOpening Device Driver...\n");
    fileDirectory = open("/dev/deviceDriver", O_RDWR);
    if (fileDirectory < 0) {
        perror("ERROR: Could not open the Device!\n");
        return errno;
    }
    else {
        printf("Device Opened Successfully!\n");
    }


    printf("\nLet's test the write function!");
    printf("\nEnter a number to be written to the Device: ");
    scanf("%s", writtenNumber);
    printf("\nData being sent to the device..\n");
    returnedValue = write(fileDirectory, writtenNumber, strlen(writtenNumber));

    //Read the data back
    printf("\nTesting..\n");
    printf("\nReading data from Device Driver..\n");
    returnedValue = read(fileDirectory, getNum, BUFFER_LENGTH);

    number = atoi(getNum);

    //use the read and write functions from the module
    returnedValue = ioctl(fileDirectory, WRITEVALUE, &number);
    returnedValue = ioctl(fileDirectory, READVALUE, &value);

    //Display the written and read data
    printf("\nThe TEST wrote the number data, %d, to the device!\n", value);
    printf("\nThe TEST read back the number data, %d, from the device!\n", value);
    printf("\nTEST COMPLETE!\n");


    printf("\n-Closing Device Driver..\n");
    printf("Device Driver closed Successfully!\n");


    //Close the fileDir
    close(fileDirectory);
    return 0;
}