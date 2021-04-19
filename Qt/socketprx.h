#ifndef SOCKETPRX_H_
#define SOCKETPRX_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <linux/stddef.h>
#include <cstdlib>
#include <algorithm>
#include <QDebug>
#include <sstream>

/* ein eigener primitver Datentyp für den Socket-Deskriptor */
#define socket_t int
using namespace std;

string Command(string cmd) {
    string data;
    FILE * stream;
    const int max_buffer = 256;
    char buffer[max_buffer];


    stream = popen(cmd.c_str(), "r");
    if (stream) {
        while (!feof(stream))
            if (fgets(buffer, max_buffer, stream) != NULL) data.append(buffer);

        pclose(stream);
    }
return data;
}

std::string split (std::string str){
    std::size_t found = str.find_last_of("/");
    return str.substr(found+1);
}

std::string Num (long Number){
     std::ostringstream ss;
     ss << Number;
     return ss.str();
}


bool senddata(int sock, void *buf, int buflen){
    unsigned char *pbuf = (unsigned char *) buf;

    while (buflen > 0){
        int num = send(sock, pbuf, buflen, 0);


        pbuf += num;
        buflen -= num;
    }

    return true;
}


bool sendfile(int sock, FILE *f){
    fseek(f, 0, SEEK_END);
    long filesize = ftell(f);
    rewind(f);

    if (filesize == EOF) return false;


    if (filesize > 0){
        char buffer[1024];
        do{
            size_t num = std::min(filesize, (long)sizeof(buffer));

            num = fread(buffer, 1, num, f);

            if (num < 1)return false;

            if (!senddata(sock, buffer, num))return false;

            filesize -= num;
        }
        while (filesize > 0);
    }
    return true;
}



bool readdata(int sock, void *buf, int buflen){
    unsigned char *pbuf = (unsigned char *) buf;

        while (buflen > 0){
            int num = recv(sock, pbuf, buflen, 0);

            if (num == 0)return false;

            pbuf += num;
            buflen -= num;
        }

        return true;
}



bool readfile(int sock, FILE *f, long filesize){
        if (filesize > 0){
            char buffer[1024];
            do{
                int num = std::min(filesize, (long)sizeof(buffer));
                if (!readdata(sock, buffer, num)) return false;

                int offset = 0;

                do{
                    size_t written = fwrite(&buffer[offset], 1, num-offset, f);

                    if (written < 1) return false;

                    offset += written;
                }
                while (offset < num);
                filesize -= num;
            }
            while (filesize > 0);
        }
        return true;
}


/* Die Funktion gibt aufgetretene Fehler aus und
 * beendet die Anwendung. */
void error_exit(const char *error_message) {
    fprintf(stderr, "%s: %s\n", error_message, strerror(errno));
    exit(EXIT_FAILURE);
}

 int create_socket( int af, int type, int protocol ) {
    socket_t sock;
    const int y = 1;
    /* Erzeuge das Socket. */
    sock = socket(af, type, protocol);
    if (sock < 0)
        error_exit("Fehler beim Anlegen eines Sockets");

    /* Mehr dazu siehe Anmerkung am Ende des Listings ... */
    setsockopt( sock, SOL_SOCKET,
                SO_REUSEADDR, &y, sizeof(int));
    return sock;
}

/* Erzeugt die Bindung an die Serveradresse,
 * (genauer gesagt an einen bestimmten Port). */
void bind_socket(socket_t *sock, unsigned long adress,
                 unsigned short port) {
   struct sockaddr_in server;
   memset( &server, 0, sizeof (server));
   server.sin_family = AF_INET;
   server.sin_addr.s_addr = htonl(adress);
   server.sin_port = htons(port);
   if (bind(*sock, (struct sockaddr*)&server,sizeof(server)) < 0)
       error_exit("Kann das Socket nicht \"binden\"");
}

/* Teile dem Socket mit, dass Verbindungswünsche
 * von Clients entgegengenommen werden. */
void listen_socket( socket_t *sock ) {
  if(listen(*sock, 5) == -1 )
      error_exit("Fehler bei listen");
}

/* Bearbeite die Verbindungswünsche von Clients.
 * Der Aufruf von accept() blockiert so lange,
 * bis ein Client Verbindung aufnimmt. */
void accept_socket( socket_t *socket, socket_t *new_socket ){
   struct sockaddr_in client;
   unsigned int len;

   len = sizeof(client);
   *new_socket=accept(*socket,(struct sockaddr *)&client, &len);
   if (*new_socket  == -1)
      error_exit("Fehler bei accept");
}

/* Baut die Verbindung zum Server auf. */
void connect_socket(socket_t *sock, char *serv_addr,
                    unsigned short port) {
   struct sockaddr_in server;
   struct hostent *host_info;
   unsigned long addr;

   memset( &server, 0, sizeof (server));
   if ((addr = inet_addr( serv_addr )) != INADDR_NONE) {
       /* argv[1] ist eine numerische IP-Adresse */
       memcpy( (char *)&server.sin_addr, &addr, sizeof(addr));
   }
   else {
       /* Für den Fall der Fälle: Wandle den
        * Servernamen bspw. "localhost" in eine IP-Adresse um. */
       host_info = gethostbyname( serv_addr );
       if (nullptr == host_info)
           error_exit("Unbekannter Server");
       memcpy( (char *)&server.sin_addr, host_info->h_addr,
               host_info->h_length);
   }
   server.sin_family = AF_INET;
   server.sin_port = htons( port );
   /* Baue die Verbindung zum Server auf. */
   if (connect(
         *sock, (struct sockaddr *)&server, sizeof( server)) < 0)
      error_exit( "Kann keine Verbindung zum Server herstellen");
}

/* Daten versenden via TCP */
void TCP_send( socket_t *sock, char *data, std::size_t size) {
   if(send( *sock, data, size, 0) == -1 )
      error_exit("Fehler bei send()");
}

/* Daten empfangen via TCP */
void TCP_recv( socket_t *sock, char *data, std::size_t size) {
    int len;
    len = recv (*sock, data, size, 0);

    if( len > 0 || len != -1 )

       data[len] = '\0';
    else
        exit(0);

}

/* Daten senden via UDP */
void UDP_send ( socket_t *sock, char *data, std::size_t size,
                char *addr, unsigned short port){
  struct sockaddr_in addr_sento;
  struct hostent *h;
  int rc;

  /* IP-Adresse des Servers überprüfen */
  h = gethostbyname(addr);
  if (h == nullptr)
     error_exit("Unbekannter Host?");

  addr_sento.sin_family = h->h_addrtype;
  memcpy ( (char *) &addr_sento.sin_addr.s_addr,
           h->h_addr_list[0], h->h_length);
  addr_sento.sin_port = htons (port);

  rc = sendto(*sock, data, size, 0,
                 (struct sockaddr *) &addr_sento,
                 sizeof (addr_sento));
  if (rc < 0)
     error_exit("Konnte Daten nicht senden - sendto()");
}

/* Daten empfangen via UDP */
void UDP_recv( socket_t *sock, char *data, std::size_t size){
   struct sockaddr_in addr_recvfrom;
   unsigned int len;
   int n;

   len = sizeof (addr_recvfrom);
   n = recvfrom ( *sock, data, size, 0,
                   (struct sockaddr *) &addr_recvfrom, &len );
   if (n < 0) {
       printf ("Keine Daten empfangen ...\n");
       return;
    }
}

/* Socket schließen */
void close_socket( int *sock ){
    close(*sock);
}

/* Unter Linux/UNIX ist nichts zu tun ... */
void cleanup(void){
   printf("Aufraeumarbeiten erledigt ...\n");
   return;
}


#endif
