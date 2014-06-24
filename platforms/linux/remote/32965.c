source: http://www.securityfocus.com/bid/34783/info
 
GnuTLS is prone to multiple remote vulnerabilities:
 
- A remote code-execution vulnerability
- A denial-of-service vulnerability
- A signature-generation vulnerability
- A signature-verification vulnerability
 
An attacker can exploit these issues to potentially execute arbitrary code, trigger denial-of-service conditions, carry out attacks against data signed with weak signatures, and cause clients to accept expired or invalid certificates from servers.
 
Versions prior to GnuTLS 2.6.6 are vulnerable.

/*
 * Small code to reproduce the CVE-2009-1416 bad DSA key problem.
 *
 * Build it using:
 *
 *  gcc -o cve-2009-1416 cve-2009-1416.c -lgnutls
 *
 * If your gnutls library is OK then running it will print 'success!'.
 *
 * If your gnutls library is buggy then running it will print 'buggy'.
 *
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include <gcrypt.h>
#include <gnutls/gnutls.h>

int
main (void)
{
  gnutls_x509_privkey_t key;
  gnutls_datum_t p, q, g, y, x;
  int ret;

  gnutls_global_init ();
  gcry_control (GCRYCTL_ENABLE_QUICK_RANDOM, 0);

  ret = gnutls_x509_privkey_init (&key);
  if (ret < 0)
    return 1;

  ret = gnutls_x509_privkey_generate (key, GNUTLS_PK_DSA, 512, 0);
  if (ret < 0)
    return 1;

  ret = gnutls_x509_privkey_export_dsa_raw (key, &p, &q, &g, &y, &x);
  if (ret < 0)
    return 1;

  if (q.size == 3 && memcmp (q.data, "\x01\x00\x01", 3) == 0)
    printf ("buggy\n");
  else
    printf ("success!\n");

  gnutls_free (p.data);
  gnutls_free (q.data);
  gnutls_free (g.data);
  gnutls_free (y.data);
  gnutls_free (x.data);

  gnutls_x509_privkey_deinit (key);
  gnutls_global_deinit ();

  return 0;
}

 