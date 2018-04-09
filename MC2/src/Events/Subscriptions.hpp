#pragma once
#include <functional>

#include "Events/Types/Events.h"
#include "Events/Types/EventData.h"

class BaseSubscription {
public:
  virtual ~BaseSubscription() {}
  virtual void send(EventData *d, void *source) const = 0;
};

template<class T, class B=T>
class ObjectSubscription : public BaseSubscription {
public:
  ObjectSubscription(T *obj, void (B::*cb)(EventData*)) : obj(obj), cb(cb), sender(NULL) {}
  virtual ~ObjectSubscription() {}
  virtual void send(EventData *d, void *source) const {
    if (sender) {
      if (sender == source) {
        (obj->*cb)(d);
      }
      return;
    }
    (obj->*cb)(d);
  }

  void setSourceFilter(void *source) {
    sender = source;
  }

private:
  T *obj;
  void (B::*cb)(EventData*);
  void *sender;
};

class FuncSubscription : public BaseSubscription {
public:
  FuncSubscription(void (*cb)(EventData*)) : cb(cb) {}
  virtual ~FuncSubscription() {}
  virtual void send(EventData *d, void *) const {
    cb(d);
  }

private:
  void (*cb)(EventData*);
};

class LambdaSubscription : public BaseSubscription {
public:
  LambdaSubscription(std::function<void (EventData *)> cb) : cb(cb) {}
  virtual ~LambdaSubscription() {}
  virtual void send(EventData *d, void *source) const {
    cb(d);
  }
  
private:
  std::function<void (EventData *)> cb;
};