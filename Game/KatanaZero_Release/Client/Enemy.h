#pragma once
#include "Character.h"

class Enemy :
    public Character
{
public :
    using Super = Character;
    virtual OBJECT_TAG::ETAG GetTag()override;
    virtual void Initialize() & noexcept override;
    virtual void Die() & abstract;
protected:
    std::shared_ptr<class Player> _Target;
};

