#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/slist.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Graph vertex.

   <b>Attributes</b>

   \arg \b object : Object associated with this vertex.

   \arg \b edges : Edges to other vertices.

   \arg <b><i>owner</i> flag</b> : Ownership flag for associated object.

   \arg <b><i>visited</i> flag</b> : Whether vertex has been \b visited.

   \author Adam McKee
   \ingroup graph
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class Vertex : public Object
{
    UTL_CLASS_DECL(Vertex, Object);

public:
    /**
       Constructor.
       \param object associated object
    */
    Vertex(const Object* object)
    {
        init(object);
    }

    /** Copy another Vertex. */
    virtual void copy(const Object& object);

    /**
       Get the key for the vertex.  If there is an associated object, return its key,
       else return Object::getKey.
    */
    virtual const Object&
    getKey() const
    {
        if (_object == nullptr)
            return super::getKey();
        else
            return *_object;
    }

    /** Add a directed edge to the given vertex. */
    void
    addDirectedEdge(const Vertex* vertex)
    {
        slist_add(_edgesHead, &_edgesTail, vertex);
    }

    /** Add an undirected edge to the given vertex. */
    void
    addUndirectedEdge(Vertex* vertex)
    {
        slist_add(_edgesHead, &_edgesTail, vertex);
        slist_add(vertex->_edgesHead, &vertex->_edgesTail, this);
    }

    /** Clear edges and associated object. */
    void
    clear()
    {
        slist_clear(_edgesHead, &_edgesTail, false);
        setObject(nullptr);
    }

    /** Return the edges. */
    const SlistNode*
    edges() const
    {
        return _edgesHead;
    }

    /**
       Find the associated vertex matching the given key.
       \return found vertex (nullptr if none found)
       \param key search key
    */
    Vertex*
    find(const Object& key) const
    {
        return utl::cast<Vertex>(slist_find(_edgesHead, key));
    }

    /** Get the associated object. */
    Object*
    getObject() const
    {
        return _object;
    }

    /** Get the \b owner flag. */
    bool
    isOwner() const
    {
        return ((_flags & flg_owner) != 0);
    }

    /** Set the \b owner flag. */
    void
    setOwner(bool owner)
    {
        if (owner)
            _flags |= flg_owner;
        else
            _flags &= ~flg_owner;
    }

    /** Get the \b visited flag. */
    bool
    isVisited() const
    {
        return ((_flags & flg_visited) != 0);
    }

    /** Set the \b visited flag. */
    void
    setVisited(bool visited)
    {
        if (visited)
            _flags |= flg_visited;
        else
            _flags &= ~flg_visited;
    }

    /**
       Remove the directed edge to the given vertex.
       \return true iff edge removed
       \param vertex other vertex
    */
    bool
    removeDirectedEdge(const Vertex* vertex)
    {
        return slist_remove(_edgesHead, &_edgesTail, *vertex, false);
    }

    /**
       Remove the undirected edge to the given vertex.
       \return true iff edge removed
       \param vertex other vertex
    */
    bool
    removeUndirectedEdge(Vertex* vertex)
    {
        return slist_remove(_edgesHead, &_edgesTail, *vertex, false, true) &&
               slist_remove(vertex->_edgesHead, &vertex->_edgesTail, self, false, true);
    }

    /** Set the associated object. */
    void
    setObject(const Object* object)
    {
        if (isOwner())
            delete _object;
        _object = const_cast<Object*>(object);
    }

private:
    enum flg_t
    {
        flg_visited = 1,
        flg_owner = 2
    };

private:
    void
    init(const Object* object = nullptr)
    {
        _object = const_cast<Object*>(object);
        _edgesHead = _edgesTail = nullptr;
        _flags = flg_owner;
    }
    void
    deInit()
    {
        clear();
    }

private:
    Object* _object;
    SlistNode* _edgesHead;
    SlistNode* _edgesTail;
    byte_t _flags;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
