#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/TCollection.h>
#include <libutl/RBtree.h>
#include <libutl/Vertex.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Graph (directed or undirected).

   A graph is defined by a set of vertices and a set of edges that connect vertices to each other.

   <b>Attributes</b>

   \arg <b>start vertex</b> : The meaning of the start vertex is user-defined.  Generally, it
   specifies the vertex where traversal of the graph should begin.

   \arg <b><i>directed</i> flag</b> : A graph may be directed or undirected.  In a directed graph,
   edges are added by calling Vertex::addDirectedEdge and removed by calling
   Vertex::removeDirectedEdge.  Likewise, in an undirected graph, edges are added by calling
   Vertex::addUndirectedEdge and removed by calling Vertex::removeUndirectedEdge.

   \author Adam McKee
   \ingroup graph
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class Graph
    : public TCollection<Vertex>
    , protected FlagsMI
{
    UTL_CLASS_DECL(Graph, TCollection<Vertex>);

public:
    /**
       Constructor.
       \param owner owner flag
       \param directed directed flag
    */
    Graph(bool owner, bool directed = true)
    {
        setCollection(new RBtree(owner));
        _start = nullptr;
        setDirected(directed);
    }

    virtual void serialize(Stream& stream, uint_t io, uint_t mode = ser_default);

    /** Remove all vertices, clear the \b start vertex. */
    void
    clear()
    {
        super::clear();
        _start = nullptr;
    }

    /// \name Accessors
    //@{
    /** Get the \b directed flag. */
    bool
    isDirected() const
    {
        return getFlag(flg_directed);
    }

    /** Set the \b directed flag. */
    void
    setDirected(bool directed)
    {
        setFlag(flg_directed, directed);
    }

    /** Return the \b start vertex. */
    Vertex*
    getStart() const
    {
        return _start;
    }

    /** Set the \b start vertex. */
    void
    setStart(Vertex* start)
    {
        _start = start;
    }
    //@}

    /// \name Edges
    //@{
    /**
       Add an edge.
       \return true if edge successfully added, false otherwise
       \param lhsKey search key for lhs vertex
       \param rhsKey search key for rhs vertex
    */
    bool addEdge(const Object& lhsKey, const Object& rhsKey);

    /**
       Remove an edge.
       \return true iff edge successfully removed, false otherwise
       \param lhsKey search key for lhs vertex
       \param rhsKey search key for rhs vertex
    */
    bool removeEdge(const Object& lhsKey, const Object& rhsKey);
    //@}
private:
    enum flg_t
    {
        flg_directed
    };
    void
    init()
    {
        setCollection(new RBtree(true));
        _start = nullptr;
        setDirected(true);
    }
    void
    deInit()
    {
    }

private:
    Vertex* _start;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
