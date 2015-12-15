/*******************************************************************************************
 *
 *  Interim code: upgrade previous db to have int's for pulse positions.
 *
 *  Author:  Gene Myers
 *  Date  :  September 2014
 *
 ********************************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "DB.h"

#ifdef HIDE_FILES
#define PATHSEP "/."
#else
#define PATHSEP "/"
#endif

typedef struct
  { int     origin; //  Well #
    uint16  beg;    //  First pulse
    uint16  end;    //  Last pulse
    int64   boff;   //  Offset (in bytes) of compressed read in 'bases' file, or offset of
                    //    uncompressed bases in memory block
    int64   coff;   //  Offset (in bytes) of compressed quiva streams in 'quiva' file
    int     flags;  //  QV of read + flags above
  } HITS_OLD;

typedef struct
  { int     origin; //  Well #
    int     beg;    //  First pulse
    int     end;    //  Last pulse
    int64   boff;   //  Offset (in bytes) of compressed read in 'bases' file, or offset of
                    //    uncompressed bases in memory block
    int64   coff;   //  Offset (in bytes) of compressed quiva streams in 'quiva' file
    int     flags;  //  QV of read + flags above
  } HITS_NEW;

int main(int argc, char *argv[])
{ HITS_DB    db;
  FILE      *nxfile, *ixfile;
  char      *pwd, *root;
  int        i;

  if (argc != 2)
    { fprintf(stderr,"Usage: %s <path:db>\n",argv[0]);
      exit (1);
    }

  pwd    = PathTo(argv[1]);
  root   = Root(argv[1],".db");
  ixfile = Fopen(Catenate(pwd,PATHSEP,root,".idx"),"r");
  nxfile = Fopen(Catenate(pwd,PATHSEP,root,".ndx"),"w");
  if (ixfile == NULL || nxfile == NULL)
    exit (1);
  free(pwd);
  free(root);

  if (fread(&db,sizeof(HITS_DB),1,ixfile) != 1)
    SYSTEM_ERROR
  fwrite(&db,sizeof(HITS_DB),1,nxfile);

  for (i = 0; i < db.oreads; i++)
    { HITS_OLD  orec;
      HITS_NEW  nrec;

      if (fread(&orec,sizeof(HITS_OLD),1,ixfile) != 1)
        SYSTEM_ERROR

      nrec.origin = orec.origin;
      nrec.beg    = orec.beg;
      nrec.end    = orec.end;
      nrec.boff   = orec.boff;
      nrec.coff   = orec.coff;
      nrec.flags  = orec.flags;

      fwrite(&nrec,sizeof(HITS_NEW),1,nxfile);
    }

  fclose(ixfile);
  fclose(nxfile);

  exit (0);
}
