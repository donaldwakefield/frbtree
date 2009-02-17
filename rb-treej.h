#ifndef INCLUDED_RB_TREEJ
#define INCLUDED_RB_TREEJ

#include <iostream>
#include <boost/smart_ptr.hpp>

namespace RedBlack
{

unsigned long dtor_count = 0;

enum Color { Red, Black };

class RBTree
{
   class Tree
   {
   public:
      Tree(unsigned long key, Color clr) :
         key_d(key),
         left_d(),
         right_d(),
         color_d(clr) {};
      ~Tree() {};

      unsigned long key_d;
      boost::shared_ptr<Tree> left_d;
      boost::shared_ptr<Tree> right_d;
      Color color_d;
   };

   typedef boost::shared_ptr<Tree> Tree_ptr;

public:

   RBTree() : root_d() {};
   ~RBTree() {
      dtor_count++;
   };

   void insert(unsigned long key) {
      root_d = insert(key, root_d);
   }

   bool member(unsigned long key) {
      return member(key, root_d);
   }

   void pretty_print() {
      pretty_print(0, root_d);
   }

private:

   Tree_ptr ins(int key, Tree_ptr tree)
   {
      if (!tree.get())
      {
         tree = Tree_ptr(new Tree(key, Red));
      }
      else if (key < tree->key_d) {
         tree->left_d = ins(key, tree->left_d);
      }
      else if (key > tree->key_d) {
         tree->right_d = ins(key, tree->right_d);
      }
      else {
         return tree;  // key is already in tree
      }

      // check for red child and red grandchild
      if (isRed(tree->left_d) && isRed(tree->left_d->left_d))
      {
         //       z           y
         //      / \         / \
         //     y   D  ==>  /   \
         //    / \         x     z
         //   x   C       / \   / \
         //  / \         A   B C   D
         // A   B
         tree = balance(tree->left_d->left_d, tree->left_d, tree,              // x,y,z
                        tree->left_d->left_d->right_d, tree->left_d->right_d); // B,C
      }
      else if (isRed(tree->left_d) && isRed(tree->left_d->right_d))
      {
         //       z           y
         //      / \         / \
         //     x   D  ==>  /   \
         //    / \         x     z
         //   A   y       / \   / \
         //      / \     A   B C   D
         //     B   C
         tree = balance(tree->left_d, tree->left_d->right_d, tree,                      // x,y,z
                        tree->left_d->right_d->left_d, tree->left_d->right_d->right_d); // B,C
      }
      else if (isRed(tree->right_d) && isRed(tree->right_d->left_d))
      {
         //     x             y
         //    / \           / \
         //   A   z    ==>  /   \
         //      / \       x     z
         //     y   D     / \   / \
         //    / \       A   B C   D
         //   B   C
         tree = balance(tree, tree->right_d->left_d, tree->right_d,                     // x,y,z
                        tree->right_d->left_d->left_d, tree->right_d->left_d->right_d); // B,C
      }
      else if (isRed(tree->right_d) && isRed(tree->right_d->right_d))
      {
         //   x               y
         //  / \             / \
         // A   y      ==>  /   \
         //    / \         x     z
         //   B   z       / \   / \
         //      / \     A   B C   D
         //     C   D
         tree = balance(tree, tree->right_d, tree->right_d->right_d,            // x,y,z
                        tree->right_d->left_d, tree->right_d->right_d->left_d); // B,C
      }

      return tree;
   }

   Tree_ptr balance(Tree_ptr x, Tree_ptr y, Tree_ptr z, Tree_ptr B, Tree_ptr C)
   {
      // Rearrange/recolor the tree as
      //       y      <== red
      //      / \
      //     /   \
      //    x     z   <== both black
      //   / \   / \
      //  A   B C   D
      //
      // Note: A and D are not passed in because already in the right place

      x->right_d = B;
      y->left_d  = x;
      y->right_d = z;
      z->left_d  = C;

      x->color_d = Black;
      y->color_d = Red;
      z->color_d = Black;

      return y;
   }

   bool isRed(Tree_ptr tree) {
      return (tree.get() && (tree->color_d == Red));
   }

   Tree_ptr insert(unsigned long key, Tree_ptr tree)
   {
      tree = ins(key, tree);
      tree->color_d = Black;
      return tree;
   }

   /*
   bool member(unsigned long key, Tree_ptr tree)
   {
      while (tree.get())
      {
         if (key < tree->key_d) {
            tree = tree->left_d;
         }
         else if (key > tree->key_d) {
            tree = tree->right_d;
         }
         else {
            return true; // key == tree->key
         }
      }

      return false;
   }
   */

   bool member(unsigned long key, Tree_ptr tree_ptr)
   {
      Tree *tree = tree_ptr.get();
      while (tree)
      {
         if (key < tree->key_d) {
            tree = tree->left_d.get();
         }
         else if (key > tree->key_d) {
            tree = tree->right_d.get();
         }
         else {
            return true; // key == tree->key
         }
      }
   }

   void pretty_print(int indent, Tree_ptr entry)
   {
      std::string tabs(indent,'\t');

      if (entry.get())
      {
         indent++;

         pretty_print(indent, entry->left_d);

         std::cout << tabs << entry->key_d << "; ";
         if (entry->color_d == Red) {
            std::cout << "Red" << std::endl;
         }
         else {
            std::cout << "Black" << std::endl;
         }

         pretty_print(indent, entry->right_d);
      }
      else {
         std::cout << tabs << "Empty" << std::endl;
      }
   }

private:

   Tree_ptr root_d;
};

};

#endif
