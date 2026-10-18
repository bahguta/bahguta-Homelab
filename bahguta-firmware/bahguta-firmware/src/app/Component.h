#pragma once

class Component
{
public:
    virtual ~Component() = default;

    virtual void setup() = 0;

    virtual void loop() = 0;
};