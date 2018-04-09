#pragma once

#include "Graphics/cgincludes.h"

class ShaderProgram;

#define ID2(t) t
#define ID(t) ID2(t)

#define make_vectors2(t, c) c(t ## 2), c(t ## 3), c(t ## 4)
#define make_vectors(t, c) make_vectors2(t, c)

#define make_array2(t) t ## V
#define make_array(t) make_array2(t)

#define make_matrices2(t, c) c(t ## 2M), c(t ## 3M), c(t ## 4M),\
                            c(t ## 2x3M), c(t ## 2x4M),\
                            c(t ## 3x2M), c(t ## 3x4M),\
                            c(t ## 4x2M), c(t ## 4x3M)
#define make_matrices(t, c) make_matrices2(t, c)

#define enumerate_types2(t) t, make_vectors(t, ID),\
                            make_array(t), make_vectors(t, make_array),\
                            make_matrices(t, ID), make_matrices(t, make_array)
#define enumerate_types(t) enumerate_types2(t)


#define setter_func0a(suffix, type) bool setValue ## suffix(type*)
#define setter_func1a(suffix, type) bool setValue ## suffix(type*, size_t)
#define setter_func2a(suffix, type) bool setValue ## suffix(type*, size_t, size_t)

#define setter_func0(suffix, type) setter_func0a(suffix, type)
#define setter_func1(suffix, type) setter_func1a(suffix, type)
#define setter_func2(suffix, type) setter_func2a(suffix, type)

#define vector_setters2(type, t) setter_func0(2##t, type); setter_func0(3##t, type); setter_func0(4##t, type)
#define vector_setters(type, t) vector_setters2(type, t)

#define array_setters2(type, t) setter_func1(t##v, type); setter_func1(2##t##v, type); setter_func1(3##t##v, type); setter_func1(4##t##v, type)
#define array_setters(type, t) array_setters2(type, t)

#define matrix_setters2(type, t) setter_func0(2m##t, type); setter_func0(3m##t, type); setter_func0(4m##t, type);\
                                 setter_func0(2x3m##t, type); setter_func0(2x4m##t, type);\
                                 setter_func0(3x2m##t, type); setter_func0(3x4m##t, type);\
                                 setter_func0(4x2m##t, type); setter_func0(4x3m##t, type)
#define matrix_setters(type, t) matrix_setters2(type, t)

#define setters2(t, type) bool setValue ## t(type); vector_setters(type, t); array_setters(type, t); matrix_setters(type, t)
#define setters(t, type) setters2(t, type)

typedef enum {
  enumerate_types(INT),
  enumerate_types(UINT),
  enumerate_types(FLOAT),
  enumerate_types(DOUBLE),
} UniformType;

class Uniform {
friend class ShaderProgram;

public:
  ~Uniform() {}

  setters(i, int);
  setters(ui, unsigned int);
  setters(f, float);
  setters(d, double);

private:
  Uniform(UniformType t, handle_t location, ShaderProgram* shader);

  handle_t location;
  ShaderProgram* shader;
  UniformType t;
};