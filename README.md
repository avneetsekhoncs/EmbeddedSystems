# embeddedsystems

Public folder - Files on target
Work folder - Files on Host
  1. As1
   
    - Part 1 - Establish Communication
      Setup a Linux machine (or virtual machine) for doing your work this semester.
      Have the target communicating with the host via the serial port using screen.
      Have the target and host on the same Ethernet network (physical or Ethernet over USB).
    
    - Part 2 - NFS and Custom Login Messages
      Mount your host's ~\cmpt433\public\ folder on the target using NFS.
      Create a script in your target's home directory named mountNFS which mounts your NFS.
      
    - Part 3 - Hello world = Der-Finger-Poken & Der-Lighten-Blinken
      On the host, create a new directory for assignment 1’s source code: ~/cmpt433/work/as1
      On the host, create a new folder in public for sharing your compiled applications to share
      them with the target via NFS:
      Create a hello program in cmpt433/work/as1/ using printf() to
      display a message of the form: "Hello embedded world, from <your name>!"
      Create a makefile that compiles your application and places the compiled file to the
      ~/cmpt433/public/myApps/ folder. Your makefile should be such that you can
      compile and deploy (place executable to the myApps/ folder) using the following single
      command: (host)$ make
  
  2. As2
