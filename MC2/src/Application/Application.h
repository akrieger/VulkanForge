//
//  Application.h
//  MC2
//
//  Created by Andrew Krieger on 11/12/16.
//  Copyright © 2016 akrieger.org. All rights reserved.
//

#pragma once

#include <atomic>
#include <memory>
#include <vector>

class Camera;
class Renderable;
class ShaderProgram;
class Window;
class World;

class Application {
public:
  int run();
  
private:
  bool init();
  
  int execute();
  
  void loop();
  void render();
  void cleanup();
  
  std::atomic<bool> running_{false};
  
  std::shared_ptr<ShaderProgram> shaderProgram_;
  std::shared_ptr<Camera> mainCamera_;
  std::shared_ptr<Window> mainWindow_;
  std::shared_ptr<World> world_;
  
  std::vector<std::shared_ptr<Renderable>> renderables_;
};
