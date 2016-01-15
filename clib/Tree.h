/////////////////////////////////////////////////////////////////////////////
//
// This generic tree container is the property of Justin Gottschlich. It may 
// be used freely in commercial software or non-commercial software without 
// explicit permission from Mr. Gottschlich. However this header file comment 
// must always appear in this header file in its entirety.
//
// You may use this source code free of charge in any environment, pending 
// you e-mail Justin (justin@nodeka.com) so he is aware of how the tree 
// container is being used and send updates as they are made.
//
// (c) 1999-2004 Justin Gottschlich
//
/////////////////////////////////////////////////////////////////////////////

#ifndef tree_header_file
#define tree_header_file

#ifndef NULL
#define NULL 0
#endif

#if WIN32
#pragma warning( push )
// Disable warning for multiple operator= defines
#pragma warning( disable : 4522 )
#pragma warning( disable : 4786 )
#endif // WIN32

namespace clib {

template <typename T>
struct LeafConcept
{
    void constraints()
    {
        if ((*a) < (*b) && (*a) == (*b))
        {
        }
    }
    T *a;
    T *b;
};

/////////////////////////////////////////////////////////////////////////////
// tree_iterator forward declaration
/////////////////////////////////////////////////////////////////////////////
template <typename T> class tree_iterator;

/////////////////////////////////////////////////////////////////////////////
// tree pair object definition
/////////////////////////////////////////////////////////////////////////////
template <typename T> class tree
{
    //BOOST_CLASS_REQUIRES(T, LeafConcept);

public:

    typedef tree_iterator<T> iterator;
    typedef const tree_iterator<T> const_iterator;

private:

    // Class data
    mutable T data_;

    // What level are we on?
    mutable size_t level_;
    mutable size_t size_;

    // Nobody gets any access to this
    mutable tree *next_;
    mutable tree *prev_;
    mutable tree *out_;
    mutable tree *in_;

    //////////////////////////////////////////////////////////////////////////
    // Removes a link to a node ... doesn't destroy the CTree, just rips it
    // out of it's current location. This is used so it can be placed elsewhere
    // without trouble.
    //////////////////////////////////////////////////////////////////////////
    void disconnect_()
    {
        // unlink this from the master node
        if (this->out_ != NULL) {

            // this->out_ is going to be called alot in succession "register" it
            register tree *out = this->out_;

            // Decrement the size of the outter level
            --(out->size_);

            if (out->in_ == this) {
                if (NULL == this->next_) {
                    // If this is the last node of this level, zap the hidden node
                    delete this->prev_;
                    out->in_ = NULL;
                }
                else {
                    // Otherwise, just reattatch the head node to the next node
                    this->prev_->next_ = this->next_;
                    this->next_->prev_ = this->prev_;
                    out->in_ = this->next_;
                }
            }
            else {
                // We should be able to do this absolutely.
                this->prev_->next_ = this->next_;
                if (NULL != this->next_) this->next_->prev_ = this->prev_;
            }
        }
        // Point to nothing
        this->next_ = this->prev_ = NULL;
    }

    //////////////////////////////////////////////////////////////////////////
    // End of the tree list, private only
    //////////////////////////////////////////////////////////////////////////
    const tree* end_() const { return (NULL); }

    //////////////////////////////////////////////////////////////////////////
    // Does the actual insert into the tree
    //////////////////////////////////////////////////////////////////////////
    tree& i_insert(tree *inTree, tree *level, bool (*pObj)(tree*, tree*))
    {
        // Do NOT move this line beyond this point. The reason is because we must
        // check to see if the node exists here because we may be removing the ONLY
        // node in the tree. If it is then NULL == level->in_. DO NOT REMOVE THIS
        //if (false == level->mDuplicates) 

        // never allow duplicate keys
        level->remove(inTree->data());

        // if there's no inner tree, make it
        if (NULL == level->in_) {
            // Dummy node, create it -- if good memory do stuff, if NULL throw
            if (tree *temp = new tree) {
                temp->next_ = inTree;
                inTree->prev_ = temp;
                level->in_ = inTree;
            }
            else throw "allocation failed";
        }
        else {

            tree *temp = level->in_->prev_;

            while (true) {
                if (NULL == temp->next_) {
                    temp->next_ = inTree;
                    inTree->prev_ = temp;
                    break;
                }
                else if ( pObj(inTree, temp->next_) ) {

                    tree *hold = temp->next_;

                    // temp -> inTree -> hold
                    temp->next_ = inTree;
                    inTree->next_ = hold;

                    // temp <- inTree <- hold
                    hold->prev_ = inTree;
                    inTree->prev_ = temp;

                    // If we just inserted on the first node, we need to make sure
                    // the in pointer goes to inTree
                    if (hold == level->in_) level->in_ = inTree;
                    break;
                }
                temp = temp->next_;
            }
        }

        inTree->out_ = level;
        ++(level->size_);

        inTree->level_ = level->level() + 1;
        return (*inTree);
    }

    //////////////////////////////////////////////////////////////////////////
    // No function object
    //////////////////////////////////////////////////////////////////////////
    tree& i_insert(tree *inTree, tree *level)
    {
        // Do NOT move this line beyond this point. The reason is because we must
        // check to see if the node exists here because we may be removing the ONLY
        // node in the tree. If it is then NULL == level->in_. DO NOT REMOVE THIS
        //if (false == level->mDuplicates) 
        level->remove(inTree->data());

        // if there's no inner tree, make it
        if (NULL == level->in_) {
            // Dummy node, create it -- if good memory do stuff, if NULL throw
            if (tree *temp = new tree) {
                temp->next_ = inTree;
                inTree->prev_ = temp;
                level->in_ = inTree;
            }
            else throw "allocation failed";
        }
        else {

            tree *temp = level->in_->prev_;

            while (true) {
                if (NULL == temp->next_) {
                    temp->next_ = inTree;
                    inTree->prev_ = temp;
                    break;
                }
                else if ( inTree->data() < temp->next_->data() ) {

                    tree *hold = temp->next_;

                    // temp -> inTree -> hold
                    temp->next_ = inTree;
                    inTree->next_ = hold;

                    // temp <- inTree <- hold
                    hold->prev_ = inTree;
                    inTree->prev_ = temp;

                    // If we just inserted on the first node, we need to make sure
                    // the in pointer goes to inTree
                    if (hold == level->in_) level->in_ = inTree;
                    break;
                }
                temp = temp->next_;
            }
        }

        inTree->out_ = level;
        ++(level->size_);

        inTree->level_ = level->level() + 1;
        return (*inTree);
    }

protected:

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    const size_t size(const tree& in) const { return in.size(); }
    const size_t level(const tree& in) const { return in.level(); }

    //////////////////////////////////////////////////////////////////////////
    // Points to the beginning of the list and sets the current
    //////////////////////////////////////////////////////////////////////////
    const_iterator begin(const tree& in) const { return iterator( *(in.in_) ); }

    //////////////////////////////////////////////////////////////////////////
    // Notice that we're returning a const tree* here and not an iterator.
    // This is because the iterator itself has a member to a pointer. Doing
    // an iterator constructor here would be less efficient than just 
    // returning a tree* which can be assigned internally inside the iterator
    // operator--(). Also because no one can call prev from a tree itself
    // (this is protected), we don't have to worry about safety issues except
    // for iterator safety.
    //////////////////////////////////////////////////////////////////////////
    const tree* prev(const tree& in) const { return (in.prev_); }

    //////////////////////////////////////////////////////////////////////////
    // Notice that we're returning a const tree* here and not an iterator.
    // This is because the iterator itself has a member to a pointer. Doing
    // an iterator constructor here would be less efficient than just 
    // returning a tree* which can be assigned internally inside the iterator
    // operator++(). Also because no one can call prev from a tree itself
    // (this is protected), we don't have to worry about safety issues except
    // for iterator safety.
    //////////////////////////////////////////////////////////////////////////
    const tree* next(const tree& in) const { return (in.next_); }

    //////////////////////////////////////////////////////////////////////////
    const_iterator in(const tree& in) const { return iterator( *(in.in_) ); }

    //////////////////////////////////////////////////////////////////////////
    const_iterator out(const tree& in) const { return iterator( *(in.out_) ); }

public:

    //////////////////////////////////////////////////////////////////////////
    // Default constructor
    //////////////////////////////////////////////////////////////////////////
    tree() : next_(0), prev_(0), in_(0), out_(0), level_(0), size_(0) {}

    //////////////////////////////////////////////////////////////////////////
    // Paired <T> constructor
    //////////////////////////////////////////////////////////////////////////
    tree(const T &inT) : data_(inT), next_(0), prev_(0), in_(0), out_(0), level_(0), size_(0) {}

    //////////////////////////////////////////////////////////////////////////
    // operator==, expects operator== has been written for both t and u
    //////////////////////////////////////////////////////////////////////////
    const bool operator==(const tree &inTree) const
    {
        if (this->data_ == inTree.data_) return true;
        return false;
    }

    //////////////////////////////////////////////////////////////////////////
    // The operator= which is a real copy, hidden and undefined
    //////////////////////////////////////////////////////////////////////////
    const tree& operator=(const tree& in)
    {
        this->clear();

        this->data_ = in.data_;
        this->copy_tree(in);

        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    // copy constructor - now visible
    //////////////////////////////////////////////////////////////////////////
    tree(const tree &in) : data_(in.data_), next_(0), prev_(0), in_(0), out_(0), 
        level_(0), size_(0) { *this = in; }

    //////////////////////////////////////////////////////////////////////////
    // destructor -- cleans out all branches, destroyed entire tree
    //////////////////////////////////////////////////////////////////////////
    virtual ~tree()
    {
        // Disconnect ourselves -- very important for decrementing the
        // size of our parent
        this->disconnect_();

        // Now get rid of our children -- but be smart about it,
        // right before we destroy it set it's out_ to NULL
        // that way Disconnect fails immediately -- much faster

        if (this->size() > 0) {
            register tree *cur = this->in_, *prev = this->in_->prev_;

            // Delete the head node
            prev->out_ = NULL;
            delete prev;

            for (; this->size_ > 0; --this->size_) {

                prev = cur;
                cur = cur->next_;

                prev->out_ = NULL;
                delete prev;
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    void copy_tree(const tree& in)
    {
        // for each branch iterate through all nodes and copy them
        for (iterator i = in.begin(); in.end() != i; ++i) {
            iterator inserted = this->insert(i.data());

            // for each node, see if there are inners - if so, copy those too
            if (i.size() != 0) inserted.tree_ptr()->copy_tree(*i.tree_ptr());
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // Returns the first element of our tree
    //////////////////////////////////////////////////////////////////////////
    const_iterator begin() const { return iterator( *(this->in_) ); }
    iterator begin() { return iterator( *(this->in_) ); }

    //////////////////////////////////////////////////////////////////////////
    // Returns end_of_iterator
    //////////////////////////////////////////////////////////////////////////
    const_iterator& end() const { return tree::iterator::end_iterator(); }

    //////////////////////////////////////////////////////////////////////////
    // Returns the first element of our tree
    //////////////////////////////////////////////////////////////////////////
    const_iterator in() const { return iterator( *(this->in_) ); }
    iterator in() { return iterator( *(this->in_) ); }

    //////////////////////////////////////////////////////////////////////////
    // Returns an iterator which steps out one level
    //////////////////////////////////////////////////////////////////////////
    const_iterator out() const { return iterator( *(this->out_) ); }
    iterator out() { return iterator( *(this->out_) ); }

    //////////////////////////////////////////////////////////////////////////
    // much like destructor with the exception that empty can be called from
    // an iterator, calling delete on an iterator will only delete the iterator
    // calling empty from an iterator will delete the tree it's iterating.
    //////////////////////////////////////////////////////////////////////////
    void clear()
    {
        // Now get rid of our children -- but be smart about it,
        // right before we destroy it set it's out_ to NULL
        // that way disconnect_ fails immediately, much faster
        if (this->size() > 0) {
            register tree *cur = this->in_, *prev = this->in_->prev_;

            // Delete the head node
            prev->out_ = NULL;
            delete prev;

            for (; this->size_ > 0; --this->size_) {

                prev = cur;
                cur = cur->next_;

                prev->out_ = NULL;
                delete prev;
            }

            // Set our inner pointer and our size to 0
            this->in_ = NULL;
            this->size_ = 0;
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // retrieves a const ref for the t member of the pair
    //////////////////////////////////////////////////////////////////////////
    T& data() { return this->data_; }

    //////////////////////////////////////////////////////////////////////////
    // sets and retrieves the t member of the pair
    //////////////////////////////////////////////////////////////////////////
    const T& data(const T &data_) const { return (this->data_ = data_); }

    //////////////////////////////////////////////////////////////////////////
    //
    //////////////////////////////////////////////////////////////////////////
    const size_t level() const { return (this->level_); }

    //////////////////////////////////////////////////////////////////////////
    //
    //////////////////////////////////////////////////////////////////////////
    const size_t size() const { return this->size_; }

    //////////////////////////////////////////////////////////////////////////
    // This creates a new tree node from parameters and then inserts it
    // Also takes a function object which can be used for sorting on inserts
    //////////////////////////////////////////////////////////////////////////
    const iterator insert(const T &inT, bool (*pObj)(tree*, tree*))
    {
        tree *myPair = new tree(inT);
        if (NULL == myPair) throw "allocation failed";
        return iterator(i_insert(myPair, this, pObj));
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    const iterator insert(const iterator &i)
    {
        tree *myPair = new tree(i.data());
        if (NULL == myPair) throw "allocation failed";
        
        return iterator(i_insert(myPair, this));
    }

    //////////////////////////////////////////////////////////////////////////
    // Insert with no function object
    //////////////////////////////////////////////////////////////////////////
    const iterator insert(const T &inT)
    {
        tree *myPair = new tree(inT);
        if (NULL == myPair) throw "allocation failed";
        return iterator(i_insert(myPair, this));
    }

    //////////////////////////////////////////////////////////////////////////
    // This takes an existing node, disconnects it from wherever it is, and then
    // inserts it into a different tree. This does not create a new node from the
    // passed in data.
    //////////////////////////////////////////////////////////////////////////
    const iterator reinsert(tree *in, bool (*pObj)(tree*, tree*))
    {
        in->disconnect_();
        return iterator(i_insert(in, this, pObj));
    }

    //////////////////////////////////////////////////////////////////////////
    // Reinsert with no function object
    //////////////////////////////////////////////////////////////////////////
    const iterator reinsert(tree *in)
    {
        in->disconnect_();
        return iterator(i_insert(in, this));
    }

    //////////////////////////////////////////////////////////////////////////
    // removes first matching t, returns true if found, otherwise false
    //////////////////////////////////////////////////////////////////////////
    const bool remove(const T &inData)
    {
        if (tree *temp = this->in_) {
            do {
                if (inData == temp->data_) {
                    delete temp;
                    return true;
                }
            } while (NULL != (temp = temp->next_) );
        }
        return false;
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    const bool erase(const iterator& i)
    {
        if (tree *temp = this->in_) {
            do {
                if (temp == i.tree_ptr()) {
                    delete temp;
                    return true;
                }
            } while (NULL != (temp = temp->next_) );
        }
        return false;
    }

    //////////////////////////////////////////////////////////////////////////
    const iterator operator[](size_t loc) const
    {
      tree *temp;
        for (temp = this->in_; loc > 0; --loc) temp = temp->next_;
        return iterator(*temp);
    }

    //////////////////////////////////////////////////////////////////////////
    iterator operator[](size_t loc)
    {
      tree *temp;
        for (temp = this->in_; loc > 0; --loc) temp = temp->next_;
        return iterator(*temp);
    }

    //////////////////////////////////////////////////////////////////////////
    // internal_only interface, can't be called even with derived objects due
    // to its direct reference to tree's private members
    //////////////////////////////////////////////////////////////////////////
    const iterator find(const T &inT) const
    {
        if (tree *temp = this->in_) {
            do {
                if (inT == temp->data_) return ( iterator(*temp) );
            } while (NULL != (temp = temp->next_) );
        }
        return tree::iterator::end_iterator();
    }

    //////////////////////////////////////////////////////////////////////////
    //
    //////////////////////////////////////////////////////////////////////////
    const iterator find(const T &inT, bool (*obj)(const T&, const T&)) const
    {
        if (tree *temp = this->in_) {
            do {
                if ( obj(inT, temp->data_) ) return ( iterator(*temp) );
            } while (NULL != (temp = temp->next_) );
        }
        return tree::iterator::end_iterator();
    }

    //////////////////////////////////////////////////////////////////////////
    // internal_only interface, can't be called even with derived objects due
    // to its direct reference to tree's private members
    //////////////////////////////////////////////////////////////////////////
    const iterator find(const T &inT, const iterator &iter) const
    {
        if (tree *temp = iter.tree_ptr()->next_) {
            do {
                if (inT == temp->data_) return ( iterator(*temp) );
            } while (NULL != (temp = temp->next_) );
        }
        return tree::iterator::end_iterator();
    }

    //////////////////////////////////////////////////////////////////////////
    //
    //////////////////////////////////////////////////////////////////////////
    const iterator find(const T &inT, const iterator &iter, bool (*obj)(const T&, const T&)) const
    {
        if (tree *temp = iter.tree_ptr()->next_) {
            do {
                if ( obj(inT, temp->data_) ) return ( iterator(*temp) );
            } while (NULL != (temp = temp->next_) );
        }
        return tree::iterator::end_iterator();
    }
};

/////////////////////////////////////////////////////////////////////////////
// Iterator for the tree
//
// Derived from tree<> only so iterator can access tree's protected 
// methods directly and implement them in the way they make sense for the 
// iterator
//
// The actual tree base members are never used (nor could they be since they
// are private to even iterator). When a tree object is created an "iterator"
// object is automatically created of the specific type. Thus forming the
// perfect relationship between the tree and the iterator, also keeping the
// template types defined on the fly for the iterator based specifically on
// the tree types which are being created.
/////////////////////////////////////////////////////////////////////////////
template <typename T>
class tree_iterator : private tree<T>
{
private:
    typedef tree<T> TreeType;

    mutable TreeType *current_;

    static tree_iterator end_of_iterator;

    //////////////////////////////////////////////////////////////////////////
    // unaccessible from the outside world
    //////////////////////////////////////////////////////////////////////////
    TreeType* operator&();
    const TreeType* operator&() const;

public:

    TreeType* tree_ptr() const { return current_; }

    //////////////////////////////////////////////////////////////////////////
    // Returns the end_of_iterator for this <T,U,V> layout, this really speeds
    // up things like if (iter != tree.end() ), for (;iter != tree.end(); )
    //////////////////////////////////////////////////////////////////////////
    static const iterator& end_iterator() { return end_of_iterator; }

    //////////////////////////////////////////////////////////////////////////
    // Default constructor
    //////////////////////////////////////////////////////////////////////////
    tree_iterator() : current_(NULL) {}
    
    //////////////////////////////////////////////////////////////////////////
    // Copy constructors for iterators
    //////////////////////////////////////////////////////////////////////////
    tree_iterator(const tree_iterator& i) : current_(i.current_) {}

    //////////////////////////////////////////////////////////////////////////
    // Copy constructor for trees
    //////////////////////////////////////////////////////////////////////////
    tree_iterator(TreeType &tree_ref) : current_(&tree_ref) {}

    //////////////////////////////////////////////////////////////////////////
    // Operator= for iterators
    //////////////////////////////////////////////////////////////////////////
    const iterator& operator=(const tree_iterator& iter)
    {
        this->current_ = iter.current_;
        return (*this);
    }

    //////////////////////////////////////////////////////////////////////////
    // Operator= for iterators
    //////////////////////////////////////////////////////////////////////////
    const iterator& operator=(const tree_iterator& iter) const
    {
        this->current_ = iter.current_;
        return (*this);
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    const iterator operator[](size_t loc) const
    { return *(this->current_)[loc]; }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    iterator operator[](size_t loc)
    { return *(this->current_)[loc]; }

    //////////////////////////////////////////////////////////////////////////
    // Operator= for trees
    //////////////////////////////////////////////////////////////////////////
    const tree_iterator& operator=(const TreeType& rhs)
    {
        this->current_ = &(const_cast< TreeType& >(rhs) );
        return (*this);
    }

    //////////////////////////////////////////////////////////////////////////
    // Destructor
    //////////////////////////////////////////////////////////////////////////
    ~tree_iterator() {};

    //////////////////////////////////////////////////////////////////////////
    // Operator equals
    //////////////////////////////////////////////////////////////////////////
    const bool operator==(const tree_iterator& rhs) const
    {
        if (this->current_ == rhs.current_) return true;
        return false;
    }

    //////////////////////////////////////////////////////////////////////////
    // Operator not equals
    //////////////////////////////////////////////////////////////////////////
    const bool operator!=(const tree_iterator& rhs) const
    { return !(*this == rhs); }

    //////////////////////////////////////////////////////////////////////////
    // operator++, prefix
    //////////////////////////////////////////////////////////////////////////
    const iterator& operator++() const
    {
        this->current_ = ( const_cast< TreeType* >
            ( this->TreeType::next( *current_ ) ) );
        return (*this);
    }

    //////////////////////////////////////////////////////////////////////////
    // operator++, postfix
    //////////////////////////////////////////////////////////////////////////
    const iterator operator++(int) const
    {
        iterator iTemp = *this;
        ++(*this);
        return (iTemp);
    }

    //////////////////////////////////////////////////////////////////////////
    // operator--
    //////////////////////////////////////////////////////////////////////////
    iterator& operator--()
    {
        this->current_ = ( const_cast< TreeType* >
            ( this->TreeType::prev( *current_ ) ) );
        return (*this);
    }

    //////////////////////////////////////////////////////////////////////////
    // Begin iteration through the tree
    //////////////////////////////////////////////////////////////////////////
    const iterator begin() const { return this->TreeType::begin( *current_ ); }
    iterator begin() { return this->TreeType::begin( *current_ ); }

    //////////////////////////////////////////////////////////////////////////
    // Return the in iterator of this tree
    //////////////////////////////////////////////////////////////////////////
    const iterator in() const { return this->TreeType::in( *current_ ); }
    iterator in() { return this->TreeType::in( *current_ ); }

    //////////////////////////////////////////////////////////////////////////
    // Return the out iterator of this tree
    //////////////////////////////////////////////////////////////////////////
    const iterator out() const { return this->TreeType::out( *current_ ); }
    iterator out() { return this->TreeType::out( *current_ ); }

    //////////////////////////////////////////////////////////////////////////
    // Are we at the end?
    //////////////////////////////////////////////////////////////////////////
    const iterator& end() const { return this->TreeType::end(); }

    //////////////////////////////////////////////////////////////////////////
    // Return the next guy
    //////////////////////////////////////////////////////////////////////////
    const iterator next() const 
    { return iterator (* const_cast< TreeType* >( this->TreeType::next( *current_ ) ) ); }

    //////////////////////////////////////////////////////////////////////////
    // Insert into the iterator's mTree
    //////////////////////////////////////////////////////////////////////////
    const iterator insert(const T& t) const
    { return this->current_->TreeType::insert(t); }

    //////////////////////////////////////////////////////////////////////////
    // Insert into the iterator's mTree (with a function object)
    //////////////////////////////////////////////////////////////////////////
    const iterator insert(const T& t, bool (*obj)(TreeType*, TreeType*)) const
    { return this->current_->TreeType::insert(t, obj); }

    //////////////////////////////////////////////////////////////////////////
    // This takes an existing node, disconnects it from wherever it is, and then
    // inserts it into a different tree. This does not create a new node from the
    // passed in data.
    //////////////////////////////////////////////////////////////////////////
    const iterator reinsert(const iterator &in, bool (*obj)(TreeType*, TreeType*)) const
    { return this->current_->TreeType::reinsert(in.current_, obj); }

    //////////////////////////////////////////////////////////////////////////
    // Reinsert with no function object
    //////////////////////////////////////////////////////////////////////////
    const iterator reinsert(const iterator &in) const
    { return this->current_->TreeType::reinsert(in.current_); }

    //////////////////////////////////////////////////////////////////////////
    // get the data of the iter
    //////////////////////////////////////////////////////////////////////////
    T& operator*() { return this->current_->data(); }
    const T& operator*() const { return this->current_->data(); }

    //////////////////////////////////////////////////////////////////////////
    // gets the t and u members of the current tree
    //////////////////////////////////////////////////////////////////////////
    T& data() { return this->current_->data(); }
    const T& data() const { return this->current_->data(); }

    //////////////////////////////////////////////////////////////////////////
    // sets and retrieves the t and u members of the pair
    //////////////////////////////////////////////////////////////////////////
    const T& data(const T &inData) const { return this->current_->data(inData); }

    //////////////////////////////////////////////////////////////////////////
    // Get the size of the current tree_iter
    //////////////////////////////////////////////////////////////////////////
    const size_t size() const { return this->TreeType::size( *current_ ); }

    //////////////////////////////////////////////////////////////////////////
    //
    //////////////////////////////////////////////////////////////////////////
    const size_t level() const { return this->TreeType::level( *current_ ); }

    //////////////////////////////////////////////////////////////////////////
    // Removes the first instance of T in the tree
    //////////////////////////////////////////////////////////////////////////
    const bool remove(const T &inT) const { return current_->remove(inT); }

    //////////////////////////////////////////////////////////////////////////
    // Finds the first instance of T in the tree
    //////////////////////////////////////////////////////////////////////////
    const iterator find(const T &inT) const { return current_->find(inT); }

    const iterator find(const T &inT, bool (*obj)(const T&, const T&)) const
    { return current_->find(inT, obj); }

    //////////////////////////////////////////////////////////////////////////
    // Finds the next instance of T based on the iterator passed in
    //////////////////////////////////////////////////////////////////////////
    const iterator find(const T &inT, const iterator &iter) const
    { return current_->find(inT, iter); }

    const iterator find(const T &inT, const iterator &iter,
        bool (*obj)(const T&, const T&)) const
    { return current_->find( inT, iter, obj ); }

    //////////////////////////////////////////////////////////////////////////
    // Empty this entire tree
    //////////////////////////////////////////////////////////////////////////
    void clear_tree() const { delete this->current_; this->current_ = NULL; }

    //////////////////////////////////////////////////////////////////////////
    // Empty this tree's children
    //////////////////////////////////////////////////////////////////////////
    void clear_children() const { this->current_->clear(); }
};

//////////////////////////////////////////////////////////////////////////
// Static iterator initialization
//////////////////////////////////////////////////////////////////////////
template <typename T>
tree_iterator<T> tree_iterator<T>::end_of_iterator;

};

#if WIN32
#pragma warning( pop )
#endif // WIN32

#endif // tree_header_file

