//------------------------------------------------------------------------------
// list.h
//
// Copyright(c) 2016, Jeff Hutchinson
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met :
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and / or other materials provided with the distribution.
//
// * Neither the name of jbl nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

#ifndef _JBL_LIST_H_
#define _JBL_LIST_H_

#include <stdlib.h>
#include <assert.h>

/**
 * A collection that represents a linked list. Each node is heap allocated when
 * an item is added and removed when it is deleted from the list. It uses the
 * global new allocator and global delete deallocator. It supports forward 
 * iteration.
 */
template<typename T>
class List
{
public:
	/**
	 * A simple structure that contains data for a link within the List.
	 */
	struct Node
	{
		/**
		 * The data contained within the node of type T.
		 */
		T data;
		
		/**
		 * The next node within the list, or nullptr to represent the end.
		 */
		Node *next;
	};
	
	/**
	 * A class that is responsible for iterating over a List.
	 * It performs forward iteration at O(n) time.
	 * @see CIterator
	 */
	class Iterator
	{
	public:
		/**
		 * Constructs an iterator.
		 * @param ptr A pointer to the start of the list.
		 */
		Iterator(Node *ptr)
		{
			mPtr = ptr;
		}
		
		/**
		 * Incriments the iterator to advance it to access the next element.
		 * @return The current iterator.
		 */
		Iterator& operator++()
		{
			mPtr = mPtr->next;
			return *this;
		}
		
		/**
		 * Compares if two iterators are at the same position.
		 * @param it The other iterator to check.
		 * @return true if both iterators are at the same position, false
		 *  otherwise.
		 */
		bool operator!=(const Iterator &it) const
		{
			return mPtr != it.mPtr;
		}
		
		/**
		 * Dereferences the current element at the current iterator position.
		 * @return The dereferenced element at the current iterator position.
		 */
		T& operator*() const
		{
			return mPtr->data;
		}
	private:
		/**
		 * A pointer to the array.
		 */
		Node *mPtr;
	};
	
	/**
	 * A class that is responsible for iterating over a List.
	 * It performs forward iteration at O(n) time.
	 * Unlike Iterator, this version is a constant iterator over the List.
	 * @see Iterator
	 */
	class CIterator
	{
	public:
		/**
		 * Constructs a constant iterator.
		 * @param ptr A pointer to the list.
		 */
		CIterator(Node *ptr)
		{
			mPtr = ptr;
		}
		
		/**
		 * Incriments the constant iterator to advance it to access the next element.
		 * @return The current constant iterator.
		 */
		const CIterator& operator++()
		{
			mPtr = mPtr->next;
			return *this;
		}
		
		/**
		 * Compares if two constant iterators are at the same position.
		 * @param it The other constant iterator to check.
		 * @return true if both constant iterators are at the same position,
		 *  false otherwise.
		 */
		const bool operator!=(const CIterator &it) const
		{
			return mPtr != it.mPtr;
		}
		
		/**
		 * Dereferences the current element at the current constant iterator position.
		 * @return The dereferenced element at the current constant iterator position.
		 */
		const T& operator*() const
		{
			return mPtr->data;
		}
		
	private:
		/**
		 * A pointer to the array.
		 */
		Node *mPtr;
	};
	
	List()
	{
		// dummy node
		mFront = new Node();
		mFront->next = nullptr;
		
		mCount = 0;
	}
	
	~List()
	{
		// we start at mFront instead of mFront->next because we can delete
		// the dummy node.
		Node *next;
		for (Node *n = mFront; n != nullptr; n = next)
		{
			next = n->next;
			delete n;
		}
	}
	
	/**
	 * Adds an item to the rear of the list.
	 * @param item the data to store within the list.
	 */
	inline void add(T item)
	{
		Node *node = new Node();
		node->data = item;
		node->next = nullptr;

		// attach to end of list
		Node *n = mFront;
		while (n->next != nullptr)
			n = n->next;
		n->next = node;
		
		++mCount;
	}
	
	/**
	 * Removes an item within the list, specified by the item itself.
	 * @param item the item to remove within the list.
	 * @return true if an item was removed, false otherwise.
	 */
	inline bool remove(const T &item)
	{
		Node *previous = mFront;
		for (Node *n = mFront->next; n != nullptr; n = n->next)
		{
			if (n->data == item)
			{
				previous->next = n->next;
				delete  n;
				--mCount;
				return true;
			}
		}
		return false;
	}
	
	/**
	 * Removes all items within the list that is equal to the item provided.
	 * @param item the item to remove within the list.
	 * @return true if at least one item was removed from the list, false
	 *  otherwise.
	 */
	inline bool removeAll(const T &item)
	{
		bool hasRemoved = false;
		bool removed = false;
		while (true)
		{
			// We write this separatly for the potential of remove() being
			// allowed to be inline instead of doing it as:
			//    while (remove(item)) removed = true;
			hasRemoved = remove(item);
			if (!hasRemoved)
				break;
			
			// We have removed at least one item, so it was successful.
			removed = true;
		}
		return removed;
	}
	
	/**
	 * Checks to see if the item is within the list.
	 * @param item The item to see if it exists within the list.
	 * @return true if the item is in the list, false otherwise.
	 */
	inline bool contains(const T &item) const
	{
		for (Node *n = mFront->next; n != nullptr; n = n->next)
		{
			if (n->data == item)
				return true;
		}
		return false;
	}
	
	/**
	 * Checks to see if the list is empty.
	 * @return true if the list is empty, false otherwise.
	 */
	inline bool isEmpty() const
	{
		return mFront->next == nullptr;
	}
	
	/**
	 * Grabs the first element in the list.
	 * @return the first element or nullptr if the list is empty.
	 */
	inline Node* front() const
	{
		return mFront->next;
	}
	
	/**
	 * Returns the count of how many elements there are in the list.
	 * @return the amount of elements within the list.
	 */
	inline unsigned int count() const
	{
		return mCount;
	}
	
	/**
	 * Grabs an iterator at the beginning of the List.
	 * @return an iterator at the beginning of the List.
	 */
	inline Iterator begin()
	{
		return Iterator(mFront->next);
	}
	
	/**
	 * Grabs an iterator at the end of the List.
	 * @return an iterator at the end of the List.
	 */
	inline Iterator end()
	{
		return Iterator(nullptr);
	}
	
	/**
	 * Grabs a constant iterator at the beginning of the List.
	 * @return a constant iterator at the beginning of the List.
	 */
	inline CIterator begin() const
	{
		return CIterator(mFront->next);
	}
	
	/**
	 * Grabs a constant iterator at the end of the List.
	 * @return a constant iterator at the end of the List.
	 */
	inline CIterator end() const
	{
		return CIterator(nullptr);
	}
	
private:
	/**
	 * A dummy node that is the start of the list.
	 * mFront->next actually holds the first element.
	 */
	Node *mFront;
	
	/**
	 * The amount of elements that are within the list.
	 */
	unsigned int mCount;
};

#endif // _JBL_LIST_H_
