#include <stdio.h>
#include "../utils/math.cpp"
#include "../utils/debug.h"

int main()
{
  v2_i vec_1 = _v2(2,4);

  vec_1.x = 5;
  vec_1.y = 10;

  v2_i vec_2 = _v2(4,1);

  v2_i vec_result = vec_1 + vec_2;
  
  _a(vec_result.x == 9 && vec_result.y == 11);

  vec_result = vec_1 * vec_2;
  
  _a(vec_result.x == 20 && vec_result.y == 10);

  v2_i vec2_3 = _v2(20,10);
  _a(vec_result == vec2_3);

  v3 v3_1 = _v3(2.0f, 10.f,20.f);
  v3 v3_2 = _v3(21.0f,10.f,33.f);

  v3 vec3_result = v3_1 + v3_2;

  _a(vec3_result.x == 23.f && vec3_result.g == 20.f && vec3_result.b == 53.f);

  _a(0);
  return 0;
}
