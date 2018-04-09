#pragma once

#include <vector>

#include "Interfaces/Renderable.hpp"

#include "Window/Window.h"

struct Rect {
  unsigned int x;
  unsigned int y;
  unsigned int width;
  unsigned int height;
};

class UI : public Renderable {
public:
  UI();
  UI(Window *win);
  virtual ~UI();

  const Rect& getRect() const;
  void setRect(const Rect& rect);
};

class ComposableUI : public UI {
public:
  void attachUI(UI *ui);

private:
  std::vector<UI *> children;
};
