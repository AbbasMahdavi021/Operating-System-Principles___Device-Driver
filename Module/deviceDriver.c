/**************************************************************
* Class:  CSC-415-03 Fall 2022
* Name: Abbas Mahdavi
* Student ID: 918345420
* GitHub UserID: AbbasMahdavi021
* Project: Assignment 6 – Device Driver
*
* File: deviceDriver.c
*
* Description: Implements device driver open, read, write, 
*              release and icotl function as well as 
*              starting and ending the driver.
**************************************************************/
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/ioctl.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

//Init Device Driver values
#define DEVICE_NAME "deviceDriver"
#define CLASS_NAME "deviceModule"
#define WRITEVALUE _IOW('a', 'a', int)
#define READVALUE _IOR('a', 'b', int)


//Init Module
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Abbas Mahdavi");
MODULE_DESCRIPTION("Device Driver");


//Init variables that will use in system calls
static int majorNumber;
static char string[1024] = {0};
static short sizeOfString;
static int value = 0;


static struct class *deviceDriver_class = NULL;
static struct device *deviceDriverDev = NULL;


//Init struct functions that will be used as DD functionalities
static int DDopen(struct inode *, struct file *);
static int DDrelease(struct inode *, struct file *);
static ssize_t DDread(struct file *, char *, size_t, loff_t *);
static ssize_t DDwrite(struct file *, const char *, size_t, loff_t *);
static long DDioctl(struct file *file, unsigned int cmd, unsigned long arg);


//Struct function that Initializes system calls
//This consists of the listed callback functions, used for file operations
static struct file_operations fops = {
        .open = DDopen,
        .read = DDread,
        .write = DDwrite,
        .unlocked_ioctl = DDioctl,
        .release = DDrelease,
};


//Init kernel mod function which is in charge of registering a class
//This also declares a magic number, defining the file type.
static int __init initDD(void) {


        printk(KERN_INFO "Initializating Device Driver!\n");
        majorNumber = register_chrdev(0, DEVICE_NAME, &fops);

        //Dynamicallt allocate a majorNumber
        if (majorNumber < 0) {
                printk(KERN_ALERT "Could not register majorNumber!\n");
                return majorNumber;
        }
        printk(KERN_INFO "Successfully registered majorNumber to: %d\n", majorNumber);
        deviceDriver_class = class_create(THIS_MODULE, CLASS_NAME);

        //This is where we register the device class!
        if (IS_ERR(deviceDriver_class)) {
                unregister_chrdev(majorNumber, DEVICE_NAME);
                printk(KERN_ALERT "Could not register Device Class!\n");
                return PTR_ERR(deviceDriver_class);
        }
        printk(KERN_INFO "Successfully registered Device Class!\n");
        deviceDriverDev = device_create(deviceDriver_class, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);

        //This is where we register the device driver
        if (IS_ERR(deviceDriverDev)) {
                class_destroy(deviceDriver_class);
                unregister_chrdev(majorNumber, DEVICE_NAME);
                printk(KERN_ALERT "Could not create Device!\n");
                return PTR_ERR(deviceDriverDev);
        }
        printk(KERN_INFO "Device created successfully!\n");
        return 0;
}



//This function wraps up and cleans the device driver code to be unloaded.
static void __exit driverExit(void) {
        //destroy the device
        device_destroy(deviceDriver_class, MKDEV(majorNumber, 0));
        //Unregister the device class and destroy
        class_unregister(deviceDriver_class);
        class_destroy(deviceDriver_class);
        //Unregister the majorNumber
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_INFO "Successfully cleaned-up functions!\n");
}


//Function to be called everytime the device is opened.
static int DDopen(struct inode *inodep, struct file *filep) {
        printk(KERN_INFO "Device opened successfully!\n");
        return 0;
}


//Function to be called everytime the device is read
//This reads the data from the device and sends it to the user.
static ssize_t DDread(struct file *filep, char *buffer, size_t len, loff_t *offset) {
        int errors = 0;
        //Send the buffer string to the user
        errors = copy_to_user(buffer, string, sizeOfString);
        if (errors == 0) {
                printk(KERN_INFO "String w/ size of %d is sent to the user!\n", sizeOfString);
                return (sizeOfString = 0);
        } else {
                printk(KERN_INFO "Error in sending data to user: %d\n", errors);
                return -EFAULT;
        }
}


//Function to be called everytime the device is being written to.
//This writes data to the device, coming from the user.
static ssize_t DDwrite(struct file *filep, const char *buffer, size_t len, loff_t *offset) {
        //Append the String
        sprintf(string, "%s", buffer);
        //Keep track of the size of string.
        sizeOfString = strlen(string);
        //print using zu, lenght modefier
        printk(KERN_INFO "Writting string w/ lenght of: %zu\n", len);
        return len;
}



//Function for implementing a device driver
//Sort of a flag for when reached, write encrypts
static long DDioctl(struct file *file, unsigned int cmd, unsigned long arg) {
        switch (cmd) {
        case WRITEVALUE:
                copy_from_user(&value, (int32_t *)arg, sizeof(value));
                printk(KERN_INFO "IO Value to be written: %d\n", value);
                break;
        case READVALUE:
                copy_to_user((int32_t *)arg, &value, sizeof(value));
                printk(KERN_INFO "IO Value to be Read: %d\n", value);
                break;
        }
        return 0;
}


//Function to be called when the device is being closed
static int DDrelease(struct inode *inodep, struct file *filep) {
        printk(KERN_INFO "Device has been closed!\n");
        return 0;
}


//Initilize the device!
module_init(initDD);
//close the device!
module_exit(driverExit);