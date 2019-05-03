#include <libutl/libutl.h>
#include <libutl/IOmux.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::IOmux);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

void
IOmux::close()
{
    _input = size_t_max;
    _streams.excise();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
IOmux::copy(const Object& rhs)
{
    auto& mux = utl::cast<IOmux>(rhs);
    super::copy(mux);
    _input = mux._input;
    _streams.clear();
    _streams.setOwner(false);
    _streams = mux._streams;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
IOmux::checkOK()
{
    super::checkOK();
    if (isOutput())
    {
        for (auto stream : _streams)
        {
            stream->checkOK();
        }
    }
    if (isInput())
    {
        if (_input == size_t_max)
        {
            throwStreamErrorEx();
        }
        _streams[_input]->checkOK();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
IOmux::remove(const Stream* stream)
{
    size_t idx = _streams.findIndex(*stream);
    if (idx == size_t_max)
        return false;
    _streams.remove(idx, true, true);
    if (_streams.empty())
        setError(true);
    if (idx == _input)
    {
        _input = size_t_max;
        setError(true);
    }
    else if (idx < _input)
    {
        --_input;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
IOmux::setInput(const Stream* stream)
{
    _input = _streams.findIndex(*stream);
    return (_input != size_t_max);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
IOmux::read(byte_t* array, size_t maxBytes, size_t minBytes)
{
    ASSERTD(_input != size_t_max);
    Stream* stream = _streams[_input];
    return stream->read(array, maxBytes, minBytes);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
IOmux::write(const byte_t* array, size_t num)
{
    for (auto stream : _streams)
    {
        stream->write(array, num);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
IOmux::init(uint_t mode, bool owner)
{
    _input = size_t_max;
    _streams.setMultiSet(false);
    _streams.setOrdering(new AddressOrdering(), sort_none);
    _streams.setOwner(owner);
    setMode(mode);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
