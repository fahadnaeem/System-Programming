# HOW TO COMPILE
```
gcc postal.c -o postal -D_FILE_OFFSET_BITS=64 -lfuse
```
## CREATE TEST FOLDER BEFORE RUNNING
Create a folder named test to mount

# RUN THE POSTAL
```
./postal test
```
# UNMOUNT POSTAL
```
fusermount -u test
