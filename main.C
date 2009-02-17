#include <rb-tree.h>
#include <time.h>
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

   T_elem<int>::Node_ptr node_ptr = T_elem<int>::make_T_elem(random());
   cout << "Initial tree:" << endl;
   pretty_print<int>(node_ptr);
   cout << "End Initial" << endl << endl;

   for (int i = 0; i < 9; ++i)
   {
      long int val = random();
      if ( !boost::get< T_elem<int> >( *node_ptr ).member(val) )
      {
         node_ptr = insert_node<int>(val % 500, node_ptr);
      }
   }

   cout << "Final" << endl;
   pretty_print<int>(node_ptr);

   RBTree<int> rint_tree;

   return 0;
}
