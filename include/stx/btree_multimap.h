// $Id$
/** \file btree_multimap.h
 * Contains the specialized B+ tree template class btree_multimap
 */

/*
 * STX B+ Tree Template Classes v0.7
 * Copyright (C) 2007 Timo Bingmann
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef _STX_BTREE_MULTIMAP_H_
#define _STX_BTREE_MULTIMAP_H_

#include <stx/btree.h>

namespace stx {

/** @brief Specialized B+ tree template class implementing STL's multimap
 * container.
 *
 * Implements the STL multimap using a B+ tree. It can be used as a drop-in
 * replacement for std::multimap. Not all asymptotic time requirements are met in
 * theory. There is no allocator template parameter, instead the class has a
 * traits class defining B+ tree properties like slots and self-verification.
 *
 * Most noteworthy difference to the default red-black implementation of
 * std::multimap is that the B+ tree does not hold key and data pair together
 * in memory. Instead each B+ tree node has two arrays of keys and data
 * values. This design directly generates many problems in implementing the
 * iterator's operator's which return value_type composition pairs.
 */
template <typename _Key, typename _Data,
	  typename _Compare = std::less<_Key>,
	  typename _Traits = btree_default_map_traits<_Key, _Data> >
class btree_multimap
{
public:
    // *** Template Parameter Types

    /// First template parameter: The key type of the btree. This is stored in
    /// inner nodes and leaves
    typedef _Key			key_type;

    /// Second template parameter: The data type associated with each
    /// key. Stored in the B+ tree's leaves
    typedef _Data			data_type;

    /// Third template parameter: Key comparison function object
    typedef _Compare			key_compare;

    /// Fourth template parameter: Traits object used to define more parameters
    /// of the B+ tree
    typedef _Traits			traits;

public:
    // *** Constructed Types

    /// Typedef of our own type
    typedef btree_multimap<key_type, data_type, key_compare, traits>	self;

    /// Construct the STL-required value_type as a composition pair of key and
    /// data types
    typedef std::pair<key_type, data_type>	value_type;

    /// Implementation type of the btree_base
    typedef stx::btree<key_type, data_type, value_type, key_compare, traits, true> btree_impl;

    /// Function class comparing two value_type pairs.
    typedef typename btree_impl::value_compare	value_compare;

    /// Size type used to count keys
    typedef typename btree_impl::size_type	size_type;

    /// Small structure containing statistics about the tree
    typedef typename btree_impl::tree_stats	tree_stats;

public:
    // *** Static Constant Options and Values of the B+ Tree

    /// Base B+ tree parameter: The number of key/data slots in each leaf
    static const unsigned short		leafslotmax =  btree_impl::leafslotmax;

    /// Base B+ tree parameter: The number of key slots in each inner node,
    /// this can differ from slots in each leaf.
    static const unsigned short		innerslotmax =  btree_impl::innerslotmax;

    /// Computed B+ tree parameter: The minimum number of key/data slots used
    /// in a leaf. If fewer slots are used, the leaf will be merged or slots
    /// shifted from it's siblings.
    static const unsigned short 	minleafslots = btree_impl::minleafslots;

    /// Computed B+ tree parameter: The minimum number of key slots used
    /// in an inner node. If fewer slots are used, the inner node will be
    /// merged or slots shifted from it's siblings.
    static const unsigned short 	mininnerslots = btree_impl::mininnerslots;

    /// Debug parameter: Enables expensive and thorough checking of the B+ tree
    /// invariants after each insert/erase operation.
    static const bool 			selfverify = btree_impl::selfverify;

    /// Debug parameter: Prints out lots of debug information about how the
    /// algorithms change the tree. Requires the header file to be compiled
    /// with BTREE_DEBUG and the key type must be std::ostream printable.
    static const bool 			debug = btree_impl::debug;

    /// Operational parameter: Allow duplicate keys in the btree.
    static const bool			allow_duplicates = btree_impl::allow_duplicates;

public:
    // *** Iterators and Reverse Iterators

    /// STL-like iterator object for B+ tree items. The iterator points to a
    /// specific slot number in a leaf.
    typedef typename btree_impl::iterator		iterator;

    /// STL-like iterator object for B+ tree items. The iterator points to a
    /// specific slot number in a leaf.
    typedef typename btree_impl::const_iterator		const_iterator;

    /// create mutable reverse iterator by using STL magic
    typedef typename btree_impl::reverse_iterator 	reverse_iterator;

    /// create constant reverse iterator by using STL magic
    typedef typename btree_impl::const_reverse_iterator	const_reverse_iterator;

private:
    // *** Tree Implementation Object

    /// The contained implementation object
    btree_impl	tree;

public:
    // *** Constructors and Destructor

    /// Default constructor initializing an empty B+ tree with the standard key
    /// comparison function
    inline btree_multimap()
	: tree()
    {
    }

    /// Constructor initializing an empty B+ tree with a special key
    /// comparison object
    inline btree_multimap(const key_compare &kcf)
	: tree(kcf)
    {
    }

    /// Constructor initializing a B+ tree with the range [first,last)
    template <class InputIterator>
    inline btree_multimap(InputIterator first, InputIterator last)
	: tree(first, last)
    {
    }

    /// Constructor initializing a B+ tree with the range [first,last) and a
    /// special key comparison object
    template <class InputIterator>
    inline btree_multimap(InputIterator first, InputIterator last, const key_compare &kcf)
	: tree(first, last, kcf)
    {
    }

    /// Frees up all used B+ tree memory pages
    inline ~btree_multimap()
    {
    }

    /// Fast swapping of two identical B+ tree objects.
    void swap(self& from)
    {
	std::swap(tree, from.tree);
    }

public:
    // *** Key and Value Comparison Function Objects

    /// Constant access to the key comparison object sorting the B+ tree
    inline key_compare key_comp() const
    {
	return tree.key_comp();
    }

    /// Constant access to a constructed value_type comparison object. required
    /// by the STL
    inline value_compare value_comp() const
    {
	return tree.value_comp();
    }

public:
    // *** Fast Destruction of the B+ Tree

    /// Frees all key/data pairs and all nodes of the tree
    void clear()
    {
	tree.clear();
    }

public:
    // *** STL Iterator Construction Functions

    /// Constructs a read/data-write iterator that points to the first slot in
    /// the first leaf of the B+ tree.
    inline iterator begin()
    {
	return tree.begin();
    }

    /// Constructs a read/data-write iterator that points to the first invalid
    /// slot in the last leaf of the B+ tree.
    inline iterator end()
    {
	return tree.end();
    }

    /// Constructs a read-only constant iterator that points to the first slot
    /// in the first leaf of the B+ tree.
    inline const_iterator begin() const
    {
	return tree.begin();
    }

    /// Constructs a read-only constant iterator that points to the first
    /// invalid slot in the last leaf of the B+ tree.
    inline const_iterator end() const
    {
	return tree.end();
    }

    /// Constructs a read/data-write reverse iterator that points to the first
    /// invalid slot in the last leaf of the B+ tree. Uses STL magic.
    inline reverse_iterator rbegin()
    {
	return tree.rbegin();
    }

    /// Constructs a read/data-write reverse iterator that points to the first
    /// slot in the first leaf of the B+ tree. Uses STL magic.
    inline reverse_iterator rend()
    {
	return tree.rend();
    }

    /// Constructs a read-only reverse iterator that points to the first
    /// invalid slot in the last leaf of the B+ tree. Uses STL magic.
    inline const_reverse_iterator rbegin() const
    {
	return tree.rbegin();
    }

    /// Constructs a read-only reverse iterator that points to the first slot
    /// in the first leaf of the B+ tree. Uses STL magic.
    inline const_reverse_iterator rend() const
    {
	return tree.rend();
    }

public:
    // *** Access Functions to the Item Count

    /// Return the number of key/data pairs in the B+ tree
    inline size_type size() const
    {
	return tree.size();
    }

    /// Returns true if there is at least one key/data pair in the B+ tree
    inline bool empty() const
    {
	return tree.empty();
    }
    
    /// Returns the largest possible size of the B+ Tree. This is just a
    /// function required by the STL standard, the B+ Tree can hold more items.
    inline size_type max_size() const
    {
	return tree.max_size();
    }

    /// Return a const reference to the current statistics.
    inline const tree_stats& get_stats() const
    {
	return tree.get_stats();
    }

public:
    // *** Standard Access Functions Querying the Tree by Descending to a Leaf

    /// Non-STL function checking whether a key is in the B+ tree. The same as
    /// (find(k) != end()) or (count() != 0).
    bool exists(const key_type &key) const
    {
	return tree.exists(key);
    }

    /// Tries to locate a key in the B+ tree and returns an iterator to the
    /// key/data slot if found. If unsuccessful it returns end().
    iterator find(const key_type &key)
    {
	return tree.find(key);
    }

    /// Tries to locate a key in the B+ tree and returns an constant iterator
    /// to the key/data slot if found. If unsuccessful it returns end().
    const_iterator find(const key_type &key) const
    {
	return tree.find(key);
    }

    /// Tries to locate a key in the B+ tree and returns the number of
    /// identical key entries found.    
    size_type count(const key_type &key) const
    {
	return tree.count(key);
    }

    /// Searches the B+ tree and returns an iterator to the first key less or
    /// equal to the parameter. If unsuccessful it returns end().
    iterator lower_bound(const key_type& key)
    {
	return tree.lower_bound(key);
    }

    /// Searches the B+ tree and returns an constant iterator to the first key less or
    /// equal to the parameter. If unsuccessful it returns end().
    const_iterator lower_bound(const key_type& key) const
    {
	return tree.lower_bound(key);
    }

    /// Searches the B+ tree and returns an iterator to the first key greater
    /// than the parameter. If unsuccessful it returns end().
    iterator upper_bound(const key_type& key)
    {
	return tree.upper_bound(key);
    }

    /// Searches the B+ tree and returns an constant iterator to the first key
    /// greater than the parameter. If unsuccessful it returns end().
    const_iterator upper_bound(const key_type& key) const
    {
	return tree.upper_bound(key);
    }

    /// Searches the B+ tree and returns both lower_bound() and upper_bound().
    inline std::pair<iterator, iterator> equal_range(const key_type& key)
    {
	return tree.equal_range(key);
    }

    /// Searches the B+ tree and returns both lower_bound() and upper_bound().
    inline std::pair<const_iterator, const_iterator> equal_range(const key_type& key) const
    {
	return tree.equal_range(key);
    }

public:
    // *** B+ Tree Object Comparison Functions

    /// Equality relation of B+ trees of the same type. B+ trees of the same
    /// size and equal elements (both key and data) are considered
    /// equal. Beware of the random ordering of duplicate keys.
    inline bool operator==(const self &other) const
    {
	return (tree == other.tree);
    }

    /// Inequality relation. Based on operator==.
    inline bool operator!=(const self &other) const
    {
	return (tree != other.tree);
    }

    /// Total ordering relation of B+ trees of the same type. It uses
    /// std::lexicographical_compare() for the actual comparison of elements.
    inline bool operator<(const self &other) const
    {
	return (tree < other.tree);
    }

    /// Greater relation. Based on operator<.
    inline bool operator>(const self &other) const
    {
	return (tree > other.tree);
    }

    /// Less-equal relation. Based on operator<.
    inline bool operator<=(const self &other) const
    {
	return (tree <= other.tree);
    }

    /// Greater-equal relation. Based on operator<.
    inline bool operator>=(const self &other) const
    {
	return (tree >= other.tree);
    }

public:
    /// *** Fast Copy: Assign Operator and Copy Constructors

    /// Assignment operator. All the key/data pairs are copied
    inline self& operator= (const self &other)
    {
	if (this != &other)
	{
	    tree = other.tree;
	}
	return *this;
    }

    /// Copy constructor. The newly initialized B+ tree object will contain a
    /// copy or all key/data pairs.
    inline btree_multimap(const self &other)
	: tree(other.tree)
    {
    }
    
public:
    // *** Public Insertion Functions

    /// Attempt to insert a key/data pair into the B+ tree. As this tree allows
    /// duplicates insertion never fails.
    inline iterator insert(const value_type& x)
    {
	return tree.insert2(x.first, x.second).first;
    }
    
    /// Attempt to insert a key/data pair into the B+ tree. Beware that if
    /// key_type == data_type, then the template iterator insert() is called
    /// instead. As this tree allows duplicates insertion never fails.
    inline iterator insert(const key_type& key, const data_type& data)
    {
	return tree.insert2(key, data).first;
    }

    /// Attempt to insert a key/data pair into the B+ tree. This function is the
    /// same as the other insert, however if key_type == data_type then the
    /// non-template function cannot be called.  As this tree allows duplicates
    /// insertion never fails.
    inline iterator insert2(const key_type& key, const data_type& data)
    {
	return tree.insert2(key, data).first;
    }

    /// Attempt to insert a key/data pair into the B+ tree. The iterator hint
    /// is currently ignored by the B+ tree insertion routine.
    inline iterator insert(iterator hint, const value_type &x)
    {
	return tree.insert2(hint, x.first, x.second);
    }

    /// Attempt to insert a key/data pair into the B+ tree. The iterator hint is
    /// currently ignored by the B+ tree insertion routine.
    inline iterator insert2(iterator hint, const key_type& key, const data_type& data)
    {
	return tree.insert2(hint, key, data);
    }

    /// Attempt to insert the range [first,last) of value_type pairs into the B+
    /// tree. Each key/data pair is inserted individually.
    template <typename InputIterator>
    inline void insert(InputIterator first, InputIterator last)
    {
	return tree.insert(first, last);
    }

public:
    // *** Public Erase Functions

    /// Erases one (the first) of the key/data pairs associated with the given
    /// key.
    bool erase_one(const key_type &key)
    {
	return tree.erase_one(key);
    }

    /// Erases all the key/data pairs associated with the given key. This is
    /// implemented using erase_one() and thus not very efficient.
    size_type erase(const key_type &key)
    {
	return tree.erase(key);
    }

#ifdef BTREE_TODO
    /// Erase the key/data pair referenced by the iterator.
    void erase(iterator iter)
    {

    }
#endif

#ifdef BTREE_TODO
    /// Erase all key/data pairs in the range [first,last). This function is
    /// currently not implemented by the B+ Tree.
    void erase(iterator /* first */, iterator /* last */)
    {
	abort();
    }
#endif

#ifdef BTREE_DEBUG
public:
    // *** Debug Printing

    /// Print out the B+ tree structure with keys onto the given ostream. This function
    /// requires that the header is compiled with BTREE_DEBUG and that key_type
    /// is printable via std::ostream.
    void print(std::ostream &os) const
    {
	tree.print(os);
    }

    /// Print out only the leaves via the double linked list.
    void print_leaves(std::ostream &os) const
    {
	tree.print_leaves(os);
    }
#endif

public:
    // *** Verification of B+ Tree Invariants

    /// Run a thorough verification of all B+ tree invariants. The program
    /// aborts via BTREE_ASSERT() if something is wrong.
    void verify() const
    {
	tree.verify();
    }

public:

    /// Dump the contents of the B+ tree out onto an ostream as a binary
    /// image. The image contains memory pointers which will be fixed when the
    /// image is restored. For this to work your key_type and data_type must be
    /// integral types and contain no pointers or references.
    void dump(std::ostream &os) const
    {
	tree.dump(os);
    }

    /// Restore a binary image of a dumped B+ tree from an istream. The B+ tree
    /// pointers are fixed using the dump order. For dump and restore to work
    /// your key_type and data_type must be integral types and contain no
    /// pointers or references. Returns true if the restore was successful.
    bool restore(std::istream &is)
    {
	return tree.restore(is);
    }
};

} // namespace stx

#endif // _STX_BTREE_MULTIMAP_H_
