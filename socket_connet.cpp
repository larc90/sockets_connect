#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include "stdtypes.h"
#include "socket_connect.h"

/* Function:     Create_Socket */
/** @brief       Create a new file descriptor for local socket.
 **
 ** @param[in]   None
 ** @param[out]  None
 ** @return      Status - TRUE/FALSE
 **/
boolean Connection_Utilities::Create_Socket () {
  boolean Status = TRUE;

  // Define local socket descriptor: IPv4, TCP and Internet Protocol
  socket_local_desc = socket (AF_INET, SOCK_STREAM, 0);
  if (socket_local_desc < 0) {
    cout << "  ERROR: socket descriptor creation failed\n";
    Status = FALSE;
  }

  return (Status);
}

/* Function:     Config_Server */
/** @brief       Set server port/IP.
 **
 ** @param[in]   Port - Socket Port assigned
 ** @param[out]  Ip_Addr - IPv4 Address assigned
 ** @return      None
 **/
void Connection_Utilities::Config_Server (u16 Port, const s8* Ip_Addr) {
  socket_server_addrlen = sizeof (socket_server_addr);

  memset (&socket_server_addr, '0', socket_server_addrlen);

  // Define IP and port number for socket server
  socket_server_addr.sin_family = AF_INET;
  inet_pton (AF_INET, Ip_Addr, &socket_server_addr.sin_addr); // Convert IPv4 and IPv6 addresses from text to binary form
  socket_server_addr.sin_port = htons(Port);  // Converts to network byte order 
}

/* Function:     bind_server */
/** @brief       Bind socket to server port/IP
 **
 ** @param[in]   None
 ** @param[out]  None
 ** @return      Status - TRUE/FALSE
 **/
boolean Connection_Utilities::bind_server () {
  boolean Status = TRUE;

  if (bind (socket_local_desc, (const struct sockaddr*) &socket_server_addr, 
            (socklen_t) socket_server_addrlen) < 0) {
    cout << "  ERROR: Socket bind failed\n";
    Status = FALSE;
  }

  return (Status);
}

/* Function:     listen_client */
/** @brief       Server 'listens' for new connections.
 **
 ** @param[in]   Max_queue - Maximum client connections in queue.
 ** @param[out]  None
 ** @return      Status - TRUE/FALSE
 **/
boolean Connection_Utilities::listen_client (s32 Max_queue) {
  boolean Status = TRUE;

  if (listen (socket_local_desc, Max_queue) < 0) {
    cout << "  ERROR: Something went wrong when listening for client\n";
    Status = FALSE;
  }

  return (Status);
}

/* Function:     accept_client */
/** @brief       Server accepts one of the connections in queue and creates a 
 **              new (remote) socket for that purpose. 
 **
 ** @param[in]   None
 ** @param[out]  None
 ** @return      Status - TRUE/FALSE
 **/
boolean Connection_Utilities::accept_client () {
  boolean Status = TRUE;

  socket_remote_desc = accept (socket_local_desc, 
                               (struct sockaddr*) &socket_server_addr, 
                               (socklen_t*) &socket_server_addrlen);

  if (socket_remote_desc < 0) {
    cout << " ERROR: No remote connection found\n";
    Status = FALSE;
  }

  return (Status);
}

/* Function:     connect_server */
/** @brief       Client attepmts to connect with server and uses local socket
 **              to keep the new connection.
 **
 ** @param[in]   None
 ** @param[out]  None
 ** @return      Status - TRUE/FALSE
 **/
boolean Connection_Utilities::connect_server () {
  boolean Status = TRUE;

  if (connect (socket_local_desc, (const struct sockaddr*) &socket_server_addr,
               socket_server_addrlen) < 0)
  {
    cout << "  ERROR: Connection with server failed \n";
    Status = FALSE;
  }

  return (Status);
}

/* Function:     receive_img */
/** @brief       Once a connection detected, server will wait for resquest to
 **              receive new image from client and then start reception of 
 **              image packages.
 **
 ** @param[in]   Img_Buf - Buffer to stored new received segment.
 ** @param[in]   Img_Sz - Size of pointed buffer.
 ** @param[out]  None
 ** @return      valread - <1 No valid request from client; Otherwise, valid 
 **                        request and image reception.
 **/
s32 Connection_Utilities::receive_img (s8* Img_Buf, s32 Img_Sz) {
  s8 Id_Buf[ID_STR_SIZE];
  s32 valread = read (socket_remote_desc, Id_Buf, (size_t) ID_STR_SIZE);
  
  if (strncmp (Id_Buf, (const s8*) NEW_IMG_AVAILABLE_ID, (size_t) ID_STR_SIZE) != 0) {
    cout << "  ERROR: Invalid petition from client." << Id_Buf <<"\n";
    send (socket_remote_desc, (const s8*) INVALID_PETITION_ID, (size_t) ID_STR_SIZE, 0);
    valread = 0;
  }

  if (valread > 0) {
    send (socket_remote_desc, (const s8*) START_IMG_TX_ID, (size_t) ID_STR_SIZE, 0);
    cout << "Client attemps to send image\nReceiving...\n";
    // TODO: Add code for receiveing of image in chunks, for now trying with strings
    valread = read (socket_remote_desc, Img_Buf, (size_t) Img_Sz);
  }
  
  if (valread < 1) {
    cout << "  ERROR: image received incorrectly.\n";
  }

  //Read from the file into our send buffer
//  read_size = fread(buffer_img, sizeof(u8), (sizeof(buffer_img)-1), image);
  cout << "Done.\n";
  return (valread);
}

/* Function:     send_img */
/** @brief       Once a connection detected, client will send request to start
 **              sending of new image to server. If request accepted, image 
 **              will be sent in segments.
 **
 ** @param[in]   Img_Buf - Buffer to be sent.
 ** @param[in]   Img_Sz - Size of pointed buffer.
 ** @param[out]  None
 ** @return      Status - TRUE/FALSE
 **/
boolean Connection_Utilities::send_img (s8* Img_Buf, s32 Img_Sz) {
  s8 Id_Buf[ID_STR_SIZE];
  boolean Status = TRUE;

  cout << "Send petition for sending of new image...\n";
  send (socket_local_desc, (const s8*) NEW_IMG_AVAILABLE_ID, (size_t) ID_STR_SIZE, 0);
  s32 valread = read (socket_local_desc, Id_Buf, (size_t) ID_STR_SIZE);

  if (strncmp (Id_Buf, (const s8*) START_IMG_TX_ID, (size_t) ID_STR_SIZE) != 0) {
    cout << "  ERROR: Terminate comunication with server. \n";
    send (socket_remote_desc, (const s8*) TERMINATE_COM_ID, (size_t) ID_STR_SIZE, 0);
    Status = FALSE;
  }

  if (Status == TRUE) {
    cout << "Server received petition.\nTransmiting...\n";
    // TODO: Add code for sending of image in chunks, for now trying with strings
    valread = send (socket_local_desc, (const s8*) Img_Buf, (size_t) Img_Sz, 0);
  }

  if ((valread < 1) || (Status == FALSE)) {
    cout << "  ERROR: Terminate comunication with server. \n";
    Status = FALSE;
  }

  cout << "Done.\n";
  return (Status);
}

/* Function:     Close_Socket */
/** @brief       Close socket of current connection.
 **
 ** @param[in]   None
 ** @param[out]  None
 ** @return      Status - TRUE/FALSE
 **/
boolean Connection_Utilities::Close_Socket () {
  boolean Status = TRUE;

  if (close (socket_local_desc) < 0) {
    cout << "  ERROR: Socket descriptor couldn't be closed. \n";
    Status = FALSE;
  }

  return (Status);
}
