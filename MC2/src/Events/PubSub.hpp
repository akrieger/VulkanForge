#pragma once

#include <functional>
#include <list>
#include <vector>
#include <map>

#include "Graphics/cgincludes.h"

#include "Events/Types/Events.h"
#include "Events/Types/EventData.h"

class BaseSubscription;

typedef std::function<void (EventData *)> blithe_subscription_callback_t;
typedef std::function<void (void *, EventData *)> subscription_callback_t;
typedef std::multimap<EventType, subscription_callback_t>::iterator subscription_t;

class PubSub {
public:
  static inline PubSub& get() {
    return singleton;
  }

  void pub(EventType e, EventData *d);

  void pub(EventType e, EventData *d, void *source);
  
  subscription_t sub(EventType e, subscription_callback_t cb);
  subscription_t sub(EventType e, blithe_subscription_callback_t);
  subscription_t sub(EventType e, void *source, blithe_subscription_callback_t cb);
  
  /*
  template<class T, class B=T>
  subscription_t sub(EventType e, T* obj, void (B::*cb)(EventData *)) {
    std::function<void (EventData *)> func = std::bind(cb;
    return this->sub(e, std::bind(func, obj));
  }*/
  /*
  template<class T, class B=T>
  subscription_t sub(EventType e, void *source, B* obj, void (T::*cb)(EventData d)) {
    return this->sub(e, source, std::bind(cb, obj));
  }*/

  void unsub(subscription_t sub);
  
private:
  PubSub() {}
  ~PubSub() {}
  
  std::map<EventType, subscription_callback_t> subscriptions;
  
  PubSub& operator=(const PubSub&);
  PubSub(const PubSub&);
  
  static PubSub singleton;
};

template<class E> class Pub;

template<class E>
class Publisher {
public:
  Publisher() {}

private:
  std::list<std::function<void(const E&)>> subs;
};
