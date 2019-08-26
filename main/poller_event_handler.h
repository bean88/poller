#pragma once

class IPollerEventHandler {
public:
    virtual int handle(int type) = 0;
};