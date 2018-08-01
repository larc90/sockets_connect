#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include "stdtypes.h"
#include "ExifTool.h"
#include "socket_connect.h"

using namespace std;

/********* Private functions definition **********/

/* Function:     add_client_info */
/** @brief       Add metada on received image about IP address of client.
 **
 ** @param[in]   Img_str- Points to image path.
 ** @param[out]  None
 ** @return      Status - TRUE/FALSE
 **/
boolean  Connection_Utilities::add_client_info (const s8* Img_str) {
  boolean Status = TRUE;

  // create ExifTool object
  ExifTool *et = new ExifTool();

  // Create user comment and insert it into 'Exif_Buf'
  create_user_comment ();
  cout << Exif_Buf <<"\n";

  // Set new tag:value
  s32 cnt = et->SetNewValue ((const s8*) "User Comment", (const s8*) Exif_Buf);

  // write the tag:value in image
  et->WriteInfo(Img_str);

  // wait for exiftool to finish writing
  if ((et->Complete (10)) < 1) {
    cout << "  ERROR: executing exiftool command!\n";
    Status = FALSE;
  }

  // checking the number of update errors
  if ((et->GetSummary (SUMMARY_FILE_UPDATE_ERRORS)) > 0) {
    cout << "  ERROR: The exiftool application was not successful.\n";
    Status = FALSE;
  }

  // Delete ExifTool object
  delete et;

  return (Status);
}

/* Function:     create_user_comment */
/** @brief       Insert user comment "source_ip:<ther.source.ip.address>,is_from_local_network:<true/false>"
 **              into 'Exif_Buf'.
 **
 ** @param[in]   None.
 ** @param[out]  None
 ** @return      None.
 **/
void Connection_Utilities::create_user_comment () {

  inet_ntop ((s32) AF_INET, (const void*) &socket_client_addr.sin_addr.s_addr, 
             (s8*) client_ip_str, (socklen_t) INET_ADDRSTRLEN);

  strncpy ((s8*) &Exif_Buf[0], (const s8*) "source_ip:<", 
           (size_t) strlen((const s8*) "source_ip:<"));

  s32 str_offset = strlen ((const s8*) "source_ip:<");
  strncpy ((s8*) &Exif_Buf[str_offset], (const s8*) client_ip_str,
           (size_t) strlen((const s8*) client_ip_str));

  str_offset += strlen ((const s8*) client_ip_str);
  strncpy ((s8*) &Exif_Buf[str_offset], (const s8*) ">,is_from_local_network:<",
           (size_t) strlen((const s8*) ">,is_from_local_network:<"));

  str_offset += strlen ((const s8*) ">,is_from_local_network:<");
  if (is_local_ip () == TRUE) {
    strncpy ((s8*) &Exif_Buf[str_offset], (const s8*) "true>",
             (size_t) strlen((const s8*) "true>"));
  } else {
    strncpy ((s8*) &Exif_Buf[str_offset], (const s8*) "false>",
             (size_t) strlen((const s8*) "false>"));
  }
}

/* Function:     is_local_ip */
/** @brief       Determine is the received image comes from a local network.
 **
 ** @param[in]   None.
 ** @param[out]  None
 ** @return      Status - TRUE/FALSE
 **/
boolean Connection_Utilities::is_local_ip () {
  boolean Status = TRUE;

  inet_ntop ((s32) AF_INET, (const void*) &socket_server_addr.sin_addr.s_addr, 
             (s8*) server_ip_str, (socklen_t) INET_ADDRSTRLEN);
  cout << server_ip_str <<"\n";

  return (Status);  
}


/********* Public functions definition **********/

#ifdef __WIN32__
/* Function:     Init_WinSock */
/** @brief       Initialize socket library for windows.
 **
 ** @param[in]   None
 ** @param[out]  None
 ** @return      Status - TRUE/FALSE
 **/
boolean Connection_Utilities::Init_WinSock () {
  WORD versionWanted = MAKEWORD(1, 1);
  WSADATA wsaData;
  boolean Status = TRUE;

  if (WSAStartup (versionWanted, &wsaData) != 0) {
    cout << "  ERROR: Couldn't initialize socket library.\n";
    Status = FALSE;
  }

  return (Status);
}

/* Function:     End_Winsock */
/** @brief       End socket library for windows.
 **
 ** @param[in]   None
 ** @param[out]  None
 ** @return      None
 **/
void Connection_Utilities::End_Winsock () {
  WSACleanup();
}
#endif

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
  if (Ip_Addr != NULL) {
    inet_pton (AF_INET, Ip_Addr, (s8*) &socket_server_addr.sin_addr); // Convert IPv4 and IPv6 addresses from text to binary form
  } else {
    socket_server_addr.sin_addr.s_addr = INADDR_ANY;
  }
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
            socket_server_addrlen) < 0) {
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

  socket_client_addrlen = socket_server_addrlen;
  socket_remote_desc = accept (socket_local_desc,
                               (struct sockaddr*) &socket_client_addr,
#ifdef __WIN32__
                               &socket_client_addrlen);
#else
                               (socklen_t*) &socket_client_addrlen);
#endif

  if (socket_remote_desc < 0) {
    cout << " ERROR: No remote connection found\n";
    Status = FALSE;
  }

  return (Status);
}

/* Function:     connect_server */
/** @brief       Client attempts to connect with server and uses local socket
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
 ** @param[in]   Img_str- Points to image path.
 ** @param[out]  None
 ** @return      valread - < 1 No valid request from client; Otherwise, valid
 **                        request and image reception.
 **/
boolean Connection_Utilities::receive_img (const s8* Img_str) {
  s32 chunk_Sz;
  s32 Img_Sz;
  boolean Status = TRUE;

  // Check if some client has sent request to transfer new image
  recv (socket_remote_desc, Id_Buf, (size_t) (ID_STR_SIZE + IMG_SIZE_BYTES), 0);
  if (strncmp (Id_Buf, (const s8*) NEW_IMG_AVAILABLE_ID, (size_t) ID_STR_SIZE) != 0) {
    cout << "  ERROR: Invalid petition from client.\n";
    send (socket_remote_desc, (const s8*) INVALID_PETITION_ID, (size_t) ID_STR_SIZE, 0);
    Status = FALSE;
  }

  if (Status == TRUE) {
    // Open pointer to JPG file where image will be stored
    image = fopen (Img_str, "wb");
    strncpy ((s8*) &Img_Sz, (const s8*) &Id_Buf[3], (size_t) IMG_SIZE_BYTES);

    // Send request to client to start transference of image
    send (socket_remote_desc, (const s8*) START_IMG_TX_ID, (size_t) ID_STR_SIZE, 0);
    cout << "Client attempts to send image\nReceiving...\n";

    // Loop up to receive all image
    do {
      if (Img_Sz > MAX_CHUNK_IMG) {
        chunk_Sz = MAX_CHUNK_IMG;
      } else {
        chunk_Sz = Img_Sz;
      }

      s32 valread = recv (socket_remote_desc, Img_buf, (size_t) chunk_Sz, 0);

      s32 write_size = fwrite (Img_buf, 1, chunk_Sz, image);
      if (write_size != valread) {
        cout << "  ERROR: Issues writing image.\n";
        Status = FALSE;
        break;
      }

      Img_Sz -= chunk_Sz;
    } while (Img_Sz != 0);

    fclose (image);
  }

  if (Status == FALSE) {
    cout << "  ERROR: image received incorrectly.\n";
  } else {
    cout << "Image received.\nEnding communication.\n";
  }

  // Send message to indicate ending of transference
  send (socket_remote_desc, (const s8*) END_IMG_TX_ID, (size_t) ID_STR_SIZE, 0);

  // Stamps client's ip address into metadata of received image
  if (add_client_info (Img_str) == FALSE) {
    Status = FALSE;
  }

  return (Status);
}

/* Function:     send_img */
/** @brief       Once a connection detected, client will send request to start
 **              sending of new image to server. If request accepted, image
 **              will be sent in segments.
 **
 ** @param[in]   Img_str - Buffer to be sent.
 ** @param[out]  None
 ** @return      Status - TRUE/FALSE
 **/
boolean Connection_Utilities::send_img (const s8* Img_str) {
  s32 chunk_Sz;
  boolean Status = TRUE;

  image = fopen(Img_str, "rb");

  fseek (image, 0, SEEK_END);
  s32 Img_Sz = ftell (image);
  fseek (image, 0, SEEK_SET);

  // Send request to server to transfer a new image indicating image's size
  cout << "Send petition for sending of new image...\n";
  strncpy ((s8*) &Id_Buf[0], (const s8*) NEW_IMG_AVAILABLE_ID, (size_t) ID_STR_SIZE);
  strncpy ((s8*) &Id_Buf[3], (const s8*) &Img_Sz,  (size_t) IMG_SIZE_BYTES);
  send (socket_local_desc, (const s8*) Id_Buf, (size_t) (ID_STR_SIZE + IMG_SIZE_BYTES), 0);

  // Check if server replied request, otherwise terminate communication
  s32 valread = recv (socket_local_desc, Id_Buf, (size_t) ID_STR_SIZE, 0);
  if ((strncmp (Id_Buf, (const s8*) START_IMG_TX_ID, (size_t) ID_STR_SIZE) != 0) ||
      (valread < 0)) {
    cout << "  ERROR: Terminate communication with server. \n";
    Status = FALSE;
  }
  cout << "Server received petition.\nTransmitting...\n";

  // Start sending of image on chunks of 1024 bytes or lower (only last chunk)
  while ((Status == TRUE) && (!feof (image))) {

    if (Img_Sz > MAX_CHUNK_IMG) {
      chunk_Sz = MAX_CHUNK_IMG;
    } else {
      chunk_Sz = Img_Sz;
    }
    fread (Img_buf, (size_t) 1, (size_t) chunk_Sz, image);

    send (socket_local_desc, (const s8*) Img_buf, (size_t) chunk_Sz, 0);
  }

  fclose (image);

  if (Status == TRUE) {
    // Once full image sent, check if server finishes of receiving image
    valread = recv (socket_local_desc, Id_Buf, (size_t) ID_STR_SIZE, 0);
    if ((strncmp (Id_Buf, (const s8*) END_IMG_TX_ID, (size_t) ID_STR_SIZE) != 0) ||
        (valread < 0)) {
      cout << "  ERROR: Image was received incorrectly in server side. \n";
      Status = FALSE;
    }
  }

  if (Status == FALSE) {
    cout << "  ERROR: Troubles sending image.\n";
  } else {
    cout << "Image sent successfully!.\n";
  }

  // Terminate communication with server
  send (socket_local_desc, (const s8*) TERMINATE_COM_ID, (size_t) ID_STR_SIZE, 0);

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

#ifdef __WIN32__
  if (shutdown (socket_local_desc, SD_SEND) < 0) {
    cout << "  ERROR: shutdown of socket failed. \n";
    Status = FALSE;
  }

  if (closesocket(socket_local_desc) < 0) {
#else
  if (close (socket_local_desc) < 0) {
#endif
    cout << "  ERROR: Socket descriptor couldn't be closed. \n";
    Status = FALSE;
  }

  return (Status);
}
