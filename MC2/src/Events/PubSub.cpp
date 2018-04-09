#include "Events/Subscriptions.hpp"

#include "PubSub.hpp"

PubSub PubSub::singleton;

void PubSub::pub(EventType e, EventData *d) {
  pub(e, d, NULL);
}

void PubSub::pub(EventType e, EventData *d, void *source) {
  //XXX
  if (e == MouseMove) {
    printf("");
  }
  auto i = subscriptions.lower_bound(e);
  auto end = subscriptions.upper_bound(e);
  for (; i != end; i++) {
    i->second(source, d);
  }
}

subscription_t PubSub::sub(EventType e, subscription_callback_t cb) {
  return subscriptions.emplace(e, cb).first;
}

subscription_t PubSub::sub(EventType e, blithe_subscription_callback_t cb) {
  return this->sub(e, NULL, cb);
}

subscription_t PubSub::sub(EventType e, void *source, blithe_subscription_callback_t cb) {
  return this->sub(e, [=](void *src, EventData *d){
    if (source == src) {
      cb(d);
    }
  });
}

void PubSub::unsub(subscription_t sub) {
  subscriptions.erase(sub);
}