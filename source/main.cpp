#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "stdtypes.h"
#include "socket_connect.h"

using namespace std;


/* Function:     main */
/** @brief       Configures a client or server (depending of arguments given)
 **              for reception or sending of a image.
 **
 ** @param[in]   argc - Number of given parameter in command line.
 ** @param[in]   argv[1] - server/client.
 ** @param[in]   argv[2] - <Port number>.
 ** @param[in]   argv[3] - (Only for client) <server Ip addr>.
 ** @param[in]   argv[4] - (Only for client) <Path>/<example.jpg>
 ** @param[out]  None
 ** @return      -1 Error. Otherwise, all good.
 **/
int main (int argc, char const *argv[]) {
  Connection_Utilities Conn_Utils;
  boolean Configuration = CLIENT_CONF;

#ifdef __WIN32__
  if (Conn_Utils.Init_WinSock () == FALSE) {
    exit (EXIT_FAILURE);
  }
#endif

  // Check arguments given in command line
  if (argc < 3) {
    cout << "  ERROR: Bad number of arguments given. \n";
    exit (EXIT_FAILURE);
  }

  if (strncmp (argv[1], (const s8*) ("server"), strlen ((const s8*) "server")) == 0) {
    Configuration = SERVER_CONF;
  }

  // Create local socket
  if (Conn_Utils.Create_Socket() == FALSE) {
    exit (EXIT_FAILURE);
  }

  // Run code as a server
  if (Configuration == SERVER_CONF) {

    // Configure Server
    Conn_Utils.Config_Server (argv[2], NULL);

    // Bind the server to the IP address and port number specified
    if (Conn_Utils.bind_server () == FALSE) {
      exit (EXIT_FAILURE);
    }

    // Look for some client connection
    if (Conn_Utils.listen_client (QUEUE_CONNECTIONS) == FALSE) {
      exit (EXIT_FAILURE);
    }

    // Accept connection from client
    if (Conn_Utils.accept_client () == FALSE) {
      exit (EXIT_FAILURE);
    }

    // Receive image from client
    if (Conn_Utils.receive_img ((const s8*) DEFAULT_SERVER_IMG_PATH) == FALSE) {
      exit (EXIT_FAILURE);
    }

  } else {  // Run code as a client

    // Verify the number of arguments for client is correct
    if (argc < 5) {
      cout << "  ERROR: Bad number of arguments given. \n";
      exit (EXIT_FAILURE);
    }

    // Configure Server
    Conn_Utils.Config_Server (argv[2], argv[3]);

    // Initiates a connection with server socket
    if (Conn_Utils.connect_server () == FALSE) {
      exit (EXIT_FAILURE);
    }

    // Send new image to server
    if (Conn_Utils.send_img (argv[4]) == FALSE) {
      exit (EXIT_FAILURE);
    }
  }

#ifdef __WIN32__
  Conn_Utils.End_Winsock();
#endif

  // Close opened sockets
  if (Conn_Utils.Close_Socket () == FALSE) {
    exit (EXIT_FAILURE);
  }
}
