#Introduction
This is the code required to run the sensor configuration.  It is a quick and dirty codeset which was created to quicly get data and save it to the SD card.

There are two main scripts which do all of the heavy lifting.

1. collectdata
  * saves VectorNav data to ~/IRLLibs/datastorage/vn.csv
  * saves Lidar data to ~/IRLLibs/datastorage/lidar-data.csv
  * saves pictures to the  ~/IRLLibs/datastorage/pics directory
2. cleanup
  * takes all the data in ~/IRLLibs/datastorage and saves it to the SD card under a user-specified name

--
#Running the Scripts
##Running the collectdata script

The first step is to ssh into the Jetson, make sure you have either a mac or a linux box.  If you know what you're doing then use an windows ssh client with X forwarding.  If you don't know what your doing, connect to the MichiganTechRobotics network and open the terminal.  Then, type in the following command.

```
user@computer:~$ ssh -X ubuntu@192.168.0.2
```

Now look through the datastorage and pics directories to make sure they don't contain any data.  This probably should just be added to the script.
```
ubuntu:~$ cd IRLLibs/datastorage/
ubuntu:~/IRLLibs/datastorage$ ls
ubuntu:~/IRLLibs/datastorage$ cd pics/
ubuntu:~/IRLLibs/datastorage/pics$ ls
ubuntu:~/IRLLibs/datastorage/pics$ cd ../..
```
Once you have verified there is no old-data you want to get rid of, then begin running the script.

```
ubuntu:~/IRLLibs$ ./collectdata
```

You will be prompted with a bunch of questions that ask how you want to view the data.  Just type in "yes" or "no" as a response.

The program will notify you that data is being collected.  To end the collection of all data, type "0" into the terminal and hit enter.

```
ubuntu:~/IRLLibs$ 0
```
Once your done collecting data, look through the datastorage directory again to make sure data was collected.  Use the less command to view the csv data, and the eog command to look at pictures.  When using the less command, hit "q" to quit out of the command.
```
ubuntu:~$ cd IRLLibs/datastorage/
ubuntu:~/IRLLibs/datastorage$ ls
ubuntu:~/IRLLibs/datastorage$ less vn.csv
ubuntu:~/IRLLibs/datastorage$ less lidar-data.csv
ubuntu:~/IRLLibs/datastorage$ cd pics/
ubuntu:~/IRLLibs/datastorage/pics$ ls
ubuntu:~/IRLLibs/datastorage/pics$ eog ./*.bmp &
ubuntu:~/IRLLibs/datastorage/pics$ cd ../..
```
##Moving the data to an SD card (using the cleanup script)
Now that you've collected some data, we need to move it onto the SD card.
