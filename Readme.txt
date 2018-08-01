0. Create the exe file (skycatch_test) inside a docker container (Do it only if you don´t have any ubunto OS in your system):
  0.1. Created image  and container where to build binary:
    (Inside of 1st command line -> bld_cont)
    docker build --tag bld:1.0 -f <Local path in your system>/dockerfile_build.txt .
    docker run --name bld_cont -it -t bld:1.0

  0.2. Install packages to run G++ compiler (Inside of bld_cont):
    cd /build
    g++ -c main.cpp socket_connect.cpp
    g++ -o skycatch_test -static main.o socket_connect.o
    exit
  
  0.3. To send exe and object files from bld_cont to some folder in our PC:
    docker cp bld_cont:/build/main.o <Local path in your system>\build\object
    docker cp bld_cont:/build/socket_connect.o <Local path in your system>\build\object
    docker cp bld_cont:/build/skycatch_test <Local path in your system>\build\bin

  0.4. If compilation got errors, move corrected files from local folder to bld_cont:
    docker cp C:\Users\uidj1176\Documents\Skycatch_Test\Test\source\<file> bld_cont:/build/

1. Create docker network to use:
  docker network create --subnet=<subnet Ipv4>/<subnet mask> mylocalnet
  docker network ls

  
2. Set up server:
  2.1. Create image and container for server:
    (Inside of a 2nd command line -> server_cont)
    docker build --tag server:1.0 -f <folder in your sytem>/dockerfile_server.txt .
    docker run --net mylocalnet --ip <Server Ipv4> --hostname host_jpg --name server_cont --rm -i -t server:1.0 /bin/bash

    NOTE: Make sure 'Server Ipv4' is part of 'mylocalnet'

  2.2. Run server from container:
    cd bin
    skycatch_test server <port>
    

3. Set up client:
  3.1. create image and container for client:
    (Inside of a 3rd command line -> client_cont)
    docker build --tag client:1.0 -f <folder in your sytem>/dockerfile_client.txt .
    docker run --net mylocalnet --ip <Client Ipv4> --name client_cont --rm -i -t client:1.0 /bin/bash

    NOTE: Make sure 'Client Ipv4' is part of 'mylocalnet'

  3.2 Run client from container:
    cd bin
    skycatch_test client <port> <server Ipv4> <Example.jpg>


4. Wait for connection between server and client gets complete (check messages in each docker).


5. If connection and transmission of image was successfully, verify correct image received:
  (Inside of server's docker)
  exiftool test_server.jpg --->
    User comment : source_ip:<client's ip>,is_from_local_network:<true/false>
  
  5.1 Compare test_server.jpg vs tes_client.jpg (test_server.jpg is a little bigger due to metadata insertion)
