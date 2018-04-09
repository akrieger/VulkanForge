#pragma once

#include <functional>
#include <list>
#include <memory>

template<class E>
struct ListenerHandle {
  
};

template<class E>
struct EventListener {
  using callback_t = std::function<bool(const E&)>;
  
  EventListener(callback_t cb)
    : cb(std::move(cb))
  {
  }
  
  bool operator()(const E& e) {
    return cb(e);
  }
  
  std::function<bool(const E&)> cb;
};

template<class E>
class EventEmitter {
private:
  using ListenerList = std::list<EventListener<E>>;
  
  struct CancellingHandle {
    CancellingHandle(typename ListenerList::iterator it)
    : it(std::move(it))
    {
    }
    
    typename ListenerList::iterator it;
  };

public:
  using ListenerHandle = std::unique_ptr<CancellingHandle>;
  
  static ListenerHandle sub(EventListener<E> listener) {
    listeners.emplace_back(std::move(listener));
    return std::make_unique<CancellingHandle>(--listeners.end());
  }
  
  static ListenerHandle sub(typename EventListener<E>::callback_t listener) {
    listeners.emplace_back(EventListener<E>(std::move(listener)));
    return std::make_unique<CancellingHandle>(--listeners.end());
  }
  
  static void pub(const E& e) {
    for (const auto& listener : listeners) {
      listener(e);
    }
  }
  
private:
  static ListenerList listeners;
};

template<class E>
std::list<EventListener<E>> EventEmitter<E>::listeners;
