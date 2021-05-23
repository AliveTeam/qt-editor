#pragma once

#include "model.hpp"

class IGraphicsItem
{
public:
    virtual ~IGraphicsItem() { }
    virtual void SyncInternalObject() = 0;
    virtual std::vector<UP_ObjectProperty>& GetProperties() = 0;
};
