# IoTFS: Filesystem for IoT Devices Abstraction and Remote Control

## Introduction
The IoTFS is to provide a filesystem that allows upper level applicaitons to use ordinary file API to manage and control IoT devices which conform with standards include OIC, AllJoyn and others. Thus application level code would not need to couple with particular standards and its implementations, and get rid of build, link and runtime dependencies. Existing tools include find/grep, sed/awk, and vmstat etc remains your friends, and you can even expose IoTFS remotely to other machine by using NFS or SAMBA or SSHFS etc. Beyond above benefits, IoTFS also aim to provide additional layer of security shield for IoT devices, as application level code will not aware of underlying IoT standard implementation details and we can apply filesystem ACL to those directories/files which mapped to IoT devices. 

The current implementation includes a framework built on top of [FUSE](http://fuse.sourceforge.net/)/[fusekit](https://code.google.com/p/fusekit/), a plugin system built on top of [C++ Micro Services](https://github.com/CppMicroServices/CppMicroServices) that implements OSGi service model, and an implemented plugin built on top of IoT standard named [OIC](http://openinterconnect.org/)/[IoTivity](https://github.com/iotivity/iotivity).

The whole project include majorly three part:

* iotfs-core      : This repository
* iotfs-adpaters  : The adapter code that convert one standard to another, for example ONVIF to OIC.
* iotfs-qa        : The QA framework includes unit test and conformance test currently implemented. 

## How to Start

### Environment
The IoTFS was built and tested under Ubuntu 14.04 amd64 system.

### Build
1. Execute ${IOTFS}/build/setup.ubuntu.sh. This script will install required packages for build iotfs and iotfs-qa code. Assume current working directory is under ${IOTFS} which you git cloned.
```
./build/setup.ubuntu.sh
```

2. Execute ${IOTFS}/deps/buildall.sh. This script will download and build IoTivity, C++ Micro Services and FuseKit.
```
./deps/buildall.sh
```

3. Execute ${IOTFS}/build/build.sh. This script will use cmake to build the iotfs binaries and libraries. The executable and libraries will be generated in ./bin and ./lib respectively.
```
./build/build.sh
```

### Run

1. The quick way: execute mount.sh under ${IOTFS}/bin.

2. The manual way
   2.1 Export environment variable named IOTFS_BIN_DIR to point to **${IOTFS}/bin**. The executable named iotfs under ${IOTFS}/bin will try to locate the plugins.xml under the directory specified by ${IOTFS_BIN_DIR}. If the plugins.xml hasn't been found, the IoTFS will not be correctly mounted.
   2.2 Execute ${IOTFS}/bin/iotfs ${the directory for the IoTFS mount point}, example as below:
        ```
       cd ${IOTFS}/bin
       mkdir temp
       ./iotfs ./temp
       ```
       The last command will mount iotfs onto mount point ./temp which just created by command 'mkdir temp'. You can use './iotfs -d ./temp' to launch iotfs under debug mode, so that you can see more verbose log information.

3. Verify iotfs mount correctly. First, you can use 'mount' command to see the current mounted filesystems. You should find the following that indicates IoTFS successfully mounted on ./temp.
```
iotfs on /home/lity/projects/iotfs/bin/temp type fuse.iotfs (rw,nosuid,nodev,relatime,user_id=1000,group_id=1000,default_permissions)
```
Then you can enter into 'temp' directory and there should no errors or other abnormal things happen. But right now the ${IOTFS}/bin/temp folder should not have anything in it since there is no OIC devices up running. Please follow the step 3 to build and run sample OIC device to make something appear in IoTFS

3. Build sample OIC device
   3.1 Change directory to ${IOTFS}/demo/oicfanserver
   3.2 Execute ./build.sh
   3.3 Change directory to ${IOTFS}/demo/bin, you will see two executables: client and fan
   3.4 Execute 'fan' by ./fan, then you can go back into IoTFS in directory ${IOTFS}/bin/temp and should see a directory which represent the device you just launched.



## Contributors
   | Name       | Email                     | Github id    |
   |------------|---------------------------|--------------|
   | Tianyou Li | <tianyou.li@gmail.com>    | TianyouLi    |
   | Ziqi Wu    | <applewuziqi@gmail.com>   | SatanWoo     |


