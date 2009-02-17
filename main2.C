#include <rb-tree.h>
#include <stdlib.h>
#include <iostream>

int vals[] = { 13, 14, 39, 16, 94, 12, 51, 7, 2 };

int main()
{
   using std::cout;
   using std::endl;

   time_t seed = time(0);

#ifdef DEBUG
   srandom(123456);
#else
   srandom(seed);
#endif

   // This is just a driver for template instantiation. Later, this class is
   // wrapped and never seen!

   RBTree<int> rint_tree;

   for (int i = 0; i < 9; ++i)
   {
      long int val = random();
      if ( !rint_tree.member(val) )
      {
         rint_tree.insert(val % 500);
      }
   }

   cout << "Final" << endl << endl;

   rint_tree.dump();

   return 0;
}
