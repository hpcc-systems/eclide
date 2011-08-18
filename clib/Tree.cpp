#include "StdAfx.h"

#include "tree.h"
#include "clib.h"

int kMaxDepth = 5;
int kMaxLeaves = 20;

class Leaf : public CUnknown
{
protected:
	int m_value;

public:
	IMPLEMENT_CUNKNOWN;

	Leaf() : m_value(0)
	{
	}

	Leaf(int value) : m_value(value)
	{
	}

	int value() const
	{
		return m_value;
	}

	bool operator < (const Leaf & l) const
	{
		return (m_value < l.value());
	}

	bool operator == (const Leaf & l) const
	{
		return (m_value == l.value());
	}
};

typedef StlLinked<Leaf> LeafAdapt;

void fun2()
{
	clib::tree<LeafAdapt> leafTree;
}

void fun()
{
   // the tree containers all sit inside the core namespace
   using namespace clib;
   //using namespace nTreeData;

   tree<Leaf> leafTree;

   ////////////////////////////////////////////////////
   // create a simple leaf tree
   ////////////////////////////////////////////////////
   for (int i = 0; i < kMaxLeaves; ++i)
   {
      // insert a starter leaf
      tree<Leaf>::iterator iter = leafTree.insert(Leaf(i));

      // continue inserting children each time
      for (int depth = 0; depth < kMaxDepth; ++depth)
      {
         // insert and step into the newly created branch 
         iter = iter.insert(Leaf(depth));
      }
   }

   ////////////////////////////////////////////////////
   // output the leaf tree
   ////////////////////////////////////////////////////
   for (tree<Leaf>::const_iterator iter = leafTree.begin();
        iter != leafTree.end();
        ++iter)
   {
      //std::cout << iter.data().value() << std::endl;

      tree<Leaf>::const_iterator inner = iter;
      // tree's iterators are containers themselves - use the
      // same iterator to traverse inwards through the tree
      for (inner = inner.in(); inner != inner.end();
           inner = inner.in())
      {
         for (size_t tabs = 1; tabs < inner.level(); ++tabs)
            std::cout << "\t";

         //std::cout << (*inner).value() << std::endl;
      }
   }
}
