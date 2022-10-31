#pragma once
#include "../Logger/Logger.h"
#include <map>
#include <typeindex>
#include <list>
#include "Event.h"
#include <functional>
#include <memory>
/**
 *
 * EVENTBUS CODE by pikuma.com
 *
 *
 */
class IEventCallback {
private:
    //TODO pure virtual, HAVE TO OVERRIDE?
    virtual void Call(Event &e) = 0;

public:
    //And override a default? difference?
    virtual ~IEventCallback() = default;
    void Execute(Event &e) {
        Call(e);
    }
};


//Wrapper around function pointer
//Function belong to owner, so must also pass owner
template <typename TOwner, typename TEvent>
class EventCallback: public IEventCallback {
private:

    typedef void (TOwner::*CallbackFunction)(TEvent&);

    TOwner *ownerInstance;
    CallbackFunction callbackFunction;

    /*
     * The programmer’s intentions can be made clear to the compiler by override.
     * If the override identifier is used with a member function, the compiler makes
     * sure that the member function exists in the base class, and also the compiler
     * restricts the program to compile otherwise.
     */
    void Call(Event& e) override {
        std::invoke(callbackFunction, ownerInstance, static_cast<TEvent&>(e));
    }

public:
    EventCallback(TOwner *ownerInstance, CallbackFunction callbackFunction) {
        this->ownerInstance = ownerInstance;
        this->callbackFunction = callbackFunction;
    }

    ~EventCallback() override = default;
};

using HandlerList = std::list<std::unique_ptr<IEventCallback>>;

class EventBus
{
private:
    std::map<std::type_index, std::unique_ptr<HandlerList>> subscribers;

public:
    //TODO template arg order matters!
    template <typename TEvent, typename TOwner>
    void SubscribeToEvent(
            TOwner *ownerInstance,
            void (TOwner::*callbackFunction)(TEvent&))
    {

        if (!subscribers[typeid(TEvent)].get()) {
            //create when empty
            subscribers[typeid(TEvent)] = std::make_unique<HandlerList>();
        }

        auto subscriber = std::make_unique<EventCallback<TOwner, TEvent>>(ownerInstance, callbackFunction);

        //move to change unique pointer ownership
        subscribers[typeid(TEvent)]->push_back(std::move(subscriber));
    }

    template <typename TEvent, typename ...TArgs>
    void EmitEvent(TArgs&& ...args) {
        auto handlers = subscribers[typeid(TEvent)].get();
        if (handlers) {

            TEvent event(std::forward<TArgs>(args)...);

            for (auto it = handlers->begin(); it != handlers->end(); ++it) {
                auto handler = it->get();
                handler->Execute(event);
            }
        }
    }
};