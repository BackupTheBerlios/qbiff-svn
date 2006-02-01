//=======================================
// root of the maildir tree
//---------------------------------------
#define MY_FOLDER      "/suse/ms/Mail/"
#define MY_MAILCLIENT  "/usr/share/qbiff/readmail"
#define MY_MAILCLPRIV  "/usr/share/qbiff/readmail.private"

//=======================================
// network stuff
//---------------------------------------
#define PORT     16001
#define SERVER   "wotan.suse.de"
#define CLIENT   "wotan.suse.de"

//#define BASE_DEBUG
//=======================================
// Base Directory
//---------------------------------------
#ifndef BASE_DEBUG
#define BASEDIR "/usr/share/qbiff"
#else
#define BASEDIR "/suse/ms/Project/qbiff"
#endif

//=======================================
// certification stuff
//---------------------------------------
#define CAFILE   BASEDIR "/cert-server/rootcert.pem"
#define DH1024   BASEDIR "/cert-server/dh1024.pem"
#define DH512    BASEDIR "/cert-server/dh512.pem"
#define CADIR    NULL

//=======================================
// server/client certs
//---------------------------------------
#define SERVER_CERTFILE  BASEDIR "/cert-server/server.pem"
#define CLIENT_CERTFILE  BASEDIR "/cert-client/client.pem"

//=======================================
// pixmaps
//---------------------------------------
#define PIXINFO  BASEDIR "/pixmaps/tooltip.xpm"
#define PIXPUBL  BASEDIR "/pixmaps/public.png"
#define PIXPRIV  BASEDIR "/pixmaps/private.png" 
