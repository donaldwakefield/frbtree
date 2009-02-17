#include <iostream>
#include <boost/variant.hpp>
#include <boost/smart_ptr.hpp>

enum Color { Red, Black };

template <typename T>
class Empty
{
public:
   Empty() {
#ifdef DEBUG
      std::cout << "Constructing an Empty @ " << this << std::endl;
#endif
   }

   Empty(const Empty &e) {
#ifdef DEBUG
      std::cout << "Copy Constructing an Empty @ " << this << std::endl;
#endif
   }

   ~Empty() {
#ifdef DEBUG
      std::cout << "Destructing an Empty @ " << this << std::endl;
#endif
   }

   Color color() { return Black; }
   void color(Color) {};

   bool member(const T &) {
      return false;
   }
};

template <typename T>
class T_elem;

template <typename T>
class member_visitor : public boost::static_visitor<bool>
{
public:
   member_visitor(const T &t) : t_d(t) {};
   ~member_visitor() {};

   bool operator()(Empty<T> &elt) const {
      return elt.member(t_d);
   }

   bool operator()(T_elem<T> &elt) const {
      return elt.member(t_d);
   }

private:

   T t_d;
};

template <typename T>
class create_blk_visitor : public boost::static_visitor< typename T_elem<T>::Node_ptr >
{
  private:

   typedef typename T_elem<T>::Node     Node;
   typedef typename T_elem<T>::Node_ptr Node_ptr;


  public:

   Node_ptr operator()(Empty<T> &elt) const
   {
      // already Black
      return Node_ptr( new Node(Empty<T>()) );
   }

   Node_ptr operator()(T_elem<T> &elt) const {
      return Node_ptr(
         new Node(
            T_elem<T>(
               Black,
               elt.left(),
               elt.element(),
               elt.right()
            )
         )
      );
   }
};

template <typename T>
class T_elem
{
public:

   typedef boost::variant< Empty<T>, T_elem<T> > Node;
   typedef boost::shared_ptr< Node > Node_ptr;

public:

   T_elem(const T& elt) :
      color_d(Black),
      elt_d(elt),
      left_d( new Node),
      right_d(new Node) {};

   T_elem(const T& elt, Color c) :
      color_d(c),
      elt_d(elt),
      left_d( new Node),
      right_d(new Node) {};

   T_elem(const T_elem &tr) :
      color_d(tr.color_d),
      elt_d(tr.elt_d),
      left_d(tr.left_d),
      right_d(tr.right_d) {};

   T_elem(
      Color clr,
      Node_ptr left,
      const T& elem,
      Node_ptr right
   ) :
      color_d(clr),
      elt_d(elem),
      left_d(left),
      right_d(right) {};

   static Node_ptr make_T_elem(const T &elt) {
      return Node_ptr( new Node( T(elt) ) );
   }

   ~T_elem() {};

   Color color() { return color_d; }
   void color(Color c) { color_d = c; }

   const T &element() const { return elt_d; }

   bool member(const T& x) {
      if (x == elt_d) {
         return true;
      }

      Node *child = (x < elt_d) ? left_d.get() : right_d.get();
      return boost::apply_visitor(member_visitor<T>(x), *child);
   }

   Node_ptr left() { return left_d; }
   Node_ptr left_left()
   {
      // will throw bad_get if left is Empty
      T_elem<T> &left_elem = boost::get< T_elem<T> >(*left().get());
      return left_elem.left();
   }
   Node_ptr left_right()
   {
      // will throw bad_get if left is Empty
      T_elem<T> &left_elem = boost::get< T_elem<T> >(*left().get());
      return left_elem.right();
   }

   void reset_left(Node_ptr new_left) {
      left_d = new_left;
   }

   Node_ptr right() { return right_d; }
   Node_ptr right_left()
   {
      // will throw bad_get if right is Empty
      T_elem<T> &right_elem = boost::get< T_elem<T> >(*right().get());
      return right_elem.left();
   }
   Node_ptr right_right()
   {
      // will throw bad_get if right is Empty
      T_elem<T> &right_elem = boost::get< T_elem<T> >(*right().get());
      return right_elem.right();
   }

   void reset_right(Node_ptr new_right) {
      right_d = new_right;
   }

private:

   Color color_d;
   T elt_d;

   Node_ptr left_d;
   Node_ptr right_d;
};

template <typename T>
class test_red_visitor : public boost::static_visitor<bool>
{
public:

   bool operator()(Empty<T> &) const { return false; }

   bool operator()(T_elem<T> &elt) const { return (elt.color() == Red); }
};

template <typename T>
typename T_elem<T>::Node_ptr balance(
   Color color,
   typename T_elem<T>::Node_ptr left,
   const T &element,
   typename T_elem<T>::Node_ptr right
)
{
   using std::cout;
   using std::endl;

   typedef typename T_elem<T>::Node     Node;
   typedef typename T_elem<T>::Node_ptr Node_ptr;

   if (color == Black)
   {
      Node &left_node = *left.get();
      if ( boost::apply_visitor(test_red_visitor<T>(), left_node) )
      {
         T_elem<T> &left_elem = boost::get< T_elem<T> >(left_node); // Empty cannot be Red!

         Node &ll_node = *left_elem.left().get();
         if ( boost::apply_visitor(test_red_visitor<T>(), ll_node) )
         {
            // (T R (T R a x b) y c) z d => T R (T B a x b) y (T B c z d) // LEFT CASE

            Node_ptr new_left = boost::apply_visitor(create_blk_visitor<T>(), *left_elem.left().get());
            const T& y = left_elem.element();
            Node_ptr new_right( new Node(T_elem<T>(Black, left_elem.right(), element, right)) );

            return Node_ptr( new Node(T_elem<T>(Red, new_left, y, new_right)) );
         }

         Node &lr_node = *left_elem.right().get();
         if ( boost::apply_visitor(test_red_visitor<T>(), lr_node) )
         {
            // (T R a x (T R b y c)) z d => T R (T B a x b) y (T B c z d) // TOP CASE

            const T& x = left_elem.element();
            Node_ptr lr_ptr(left_elem.right());
            T_elem<T> &lr_elem = boost::get< T_elem<T> >(*lr_ptr.get());
            Node_ptr lrl_ptr = lr_elem.left();

            Node_ptr new_left( new Node(T_elem<T>(Black, left_elem.left(), x, lrl_ptr)) );
            Node_ptr new_right( new Node(T_elem<T>(Black, lr_elem.right(), element, right)) );

            const T& y = lr_elem.element();

            return Node_ptr( new Node(T_elem<T>(Red, new_left, y, new_right)) );
         }
      }

      Node &right_node = *right.get();
      if ( boost::apply_visitor(test_red_visitor<T>(), right_node) )
      {
         T_elem<T> &right_elem = boost::get< T_elem<T> >(right_node);

         Node &rl_node = *right_elem.left().get();
         if ( boost::apply_visitor(test_red_visitor<T>(), rl_node) )
         {
            // a x (T R (T R b y c) z d) => T R (T B a x b) y (T B c z d) // BOTTOM CASE

            Node_ptr new_left( new Node(T_elem<T>(Black, left, element, right_elem.left_left())) );
            Node_ptr new_right( new Node(T_elem<T>(Black, right_elem.left_right(), right_elem.element(), right_elem.right())) );

            const T_elem<T> &rl_elem = boost::get< T_elem<T> >(*right_elem.left().get());
            const T& y = rl_elem.element();

            return Node_ptr( new Node(T_elem<T>(Red, new_left, y, new_right)) );
         }

         Node &rr_node = *right_elem.right().get();
         if ( boost::apply_visitor(test_red_visitor<T>(), rr_node) )
         {
            // a x (T R b y (T R c z d)) => T R (T B a x b) y (T B c z d) // RIGHT CASE

            Node_ptr new_left( new Node(T_elem<T>(Black, left, element, right_elem.left())) );
            const T& y = right_elem.element();
            Node_ptr new_right = boost::apply_visitor(create_blk_visitor<T>(), *right_elem.right().get());

            return Node_ptr( new Node(T_elem<T>(Red, new_left, y, new_right)) );
         }
      }
   }

   // default case for red root
   //
   return Node_ptr( new Node(T_elem<T>(color, left, element, right)) );
}

/////////////////////////////////////////
//
// Pretty printing
//
template <typename T>
class pprint_visitor : public boost::static_visitor<>
{
public:
   pprint_visitor(int indent = 0) : indent_d(indent), format_d(indent,'\t') {};
   ~pprint_visitor() {};

   void operator()(Empty<T> &) const {
      std::cout << format_d << "Empty" << std::endl;
   }

   void operator()(T_elem<T> &elt) const
   {
      boost::apply_visitor( pprint_visitor<T>(indent_d + 1), *elt.left().get());

      std::cout << format_d << elt.element() << "; " << elt.color() << std::endl;
      
      boost::apply_visitor( pprint_visitor<T>(indent_d + 1), *elt.right().get());
   }

private:

   int indent_d;
   std::string format_d;
};

template <typename T>
void pretty_print(const typename T_elem<T>::Node_ptr root)
{
   boost::apply_visitor( pprint_visitor<T>(), *root.get() );
}

/////////////////////////////////////////
//
// Insertions
//

template <typename T>
class insert_visitor : public boost::static_visitor< typename T_elem<T>::Node_ptr >
{
private:

   typedef typename T_elem<T>::Node     Node;
   typedef typename T_elem<T>::Node_ptr Node_ptr;

public:

   insert_visitor(const T& elem) : elem_d(elem) {};
   ~insert_visitor() {};

   Node_ptr operator()(Empty<T> &elt) const {
      return Node_ptr( new Node(T_elem<T>(elem_d,Red)) ); 
   }

   Node_ptr operator()(T_elem<T> &elt) const
   {
      const T& other = elt.element();
      if (elem_d < other)
      {
         elt.reset_left( boost::apply_visitor(insert_visitor<T>(elem_d), *elt.left().get()) );
         Node_ptr retval = balance(elt.color(), elt.left(), elt.element(), elt.right());
         return retval;
      }
      else if (elem_d > other)
      {
         elt.reset_right( boost::apply_visitor(insert_visitor<T>(elem_d), *elt.right().get()) );
         Node_ptr retval = balance(elt.color(), elt.left(), elt.element(), elt.right());
         return retval;
      }

      // else (elem == other); no balancing needed

      return Node_ptr( new Node(elt) );
   }

private:

   T elem_d;
};

template <typename T>
typename T_elem<T>::Node_ptr insert_node(const T &elt, typename T_elem<T>::Node_ptr &node)
{
   typedef typename T_elem<T>::Node     Node;
   typedef typename T_elem<T>::Node_ptr Node_ptr;

   Empty<T> *test = boost::get< Empty<T> >(node.get());
   if (test) {
      return Node_ptr( new Node(T_elem<T>(elt)) );
   }
 
   const T_elem<T> &el_node = boost::get< T_elem<T> >(*node.get());

   typedef typename T_elem<T>::Node     Node;
   typedef typename T_elem<T>::Node_ptr Node_ptr;

   Node_ptr ptr = boost::apply_visitor(insert_visitor<T>(elt), *node.get());
   Node_ptr retval = boost::apply_visitor( create_blk_visitor<T>(), *ptr.get() );

#ifdef DEBUG
   const T_elem<T>& elt_out = boost::get< T_elem<T> >(*retval.get());
   std::cout << "constructed an elem with value " << elt_out.element() << std::endl;
#endif

   return retval;
}

/////////////////////////////////////////
//
// Published RBTree class
//

template <typename T>
class RBTree
{
public:

   typedef typename T_elem<T>::Node     Node;
   typedef typename T_elem<T>::Node_ptr Node_ptr;

public:

   RBTree() : root_d( new Node(Empty<T>()) ) {};
   ~RBTree() {};

   bool member(const T& t) {
      return boost::apply_visitor(member_visitor<T>(t), *root_d.get());
   }

   void insert(const T& t) {
      root_d = insert_node<T>(t, root_d);
   }

   void dump() {
      pretty_print<T>(root_d);
   }

private:

   Node_ptr root_d;
};
