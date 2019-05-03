#include <libutl/libutl.h>
#include <libutl/Graph.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_INSTANTIATE_TPL(utl::TCollection, utl::Vertex);
UTL_INSTANTIATE_TPL(utl::TBidIt, utl::Vertex);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::Graph);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Graph::serialize(Stream& stream, uint_t io, uint_t mode)
{
    super::serialize(stream, io, mode);
    if (io == io_rd)
    {
        auto object = serializeInNullable(stream, mode);
        if (object == nullptr)
            return;
        if (!object->isA(Vertex))
        {
            delete object;
            throw StreamSerializeEx(utl::clone(stream.getNamePtr()));
        }
        auto vertex = utl::cast<Vertex>(object);
        _start = find(*vertex);
        delete vertex;
    }
    else
    {
        serializeOutNullable(_start, stream, mode);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
Graph::addEdge(const Object& lhsKey, const Object& rhsKey)
{
    auto lhsVertex = find(lhsKey);
    auto rhsVertex = find(rhsKey);
    if ((lhsVertex == nullptr) || (rhsVertex == nullptr))
        return false;

    if (isDirected())
    {
        lhsVertex->addDirectedEdge(rhsVertex);
    }
    else
    {
        lhsVertex->addUndirectedEdge(rhsVertex);
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
Graph::removeEdge(const Object& lhsKey, const Object& rhsKey)
{
    auto lhsVertex = find(lhsKey);
    auto rhsVertex = find(rhsKey);
    if ((lhsVertex == nullptr) || (rhsVertex == nullptr))
    {
        return false;
    }

    if (isDirected())
    {
        return lhsVertex->removeDirectedEdge(rhsVertex);
    }
    else
    {
        return lhsVertex->removeUndirectedEdge(rhsVertex);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
