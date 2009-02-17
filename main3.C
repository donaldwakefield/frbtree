#include <rb-treep.h>
#include <time.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/time.h>
#include <iostream>
#include <vector>

const unsigned long CEILING = 134217727; // ((2**31) - 1) / 16;
const int SMPL_SZ = 1024*1024;

int main()
{
   using std::cout;
   using std::endl;
   using std::vector;

   typedef vector<unsigned long> Long_vec;
   typedef Long_vec::iterator    Long_vec_iter;

   time_t seed = time(0);

#ifdef DEBUG
   srandom(123456);
#else
   srandom(seed);
#endif

   Long_vec samples; samples.reserve(SMPL_SZ);
   for (int i = 0; i < SMPL_SZ; ++i) {
      unsigned long value = random() * CEILING;
      samples.push_back(value);
   }

   RedBlack::RBTree tree;

   cout << "Insertion loop: ";

   timeval t1;
   gettimeofday(&t1, NULL);

   for (Long_vec_iter iter = samples.begin(); iter != samples.end(); ++iter) {
      tree.insert(*iter);
   }

   timeval t2;
   gettimeofday(&t2, NULL);

   double elapsedTime = (t2.tv_sec  - t1.tv_sec)  * 1000.0;   // sec  to ms
   elapsedTime       += (t2.tv_usec - t1.tv_usec) / 1000.0;   // usec to ms
   cout << elapsedTime << " ms.\n";

   cout << "Destructor called " << RedBlack::dtor_count << " times" << endl;

   cout << "Member loop (up to " << CEILING << "): ";
   unsigned long total(0);

   gettimeofday(&t1, NULL);

   for (unsigned long k = 0; k < CEILING; ++k)
   {
      if (tree.member(k)) {
         total++;
      }
   }

   gettimeofday(&t2, NULL);

   elapsedTime  = (t2.tv_sec  - t1.tv_sec)  * 1000.0;   // sec  to ms
   elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;   // usec to ms
   cout << elapsedTime << " ms.\n";

   return 0;
}
