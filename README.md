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
