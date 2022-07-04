#pragma once
#include "types.h"

real32 max(real32 a, real32 b)
{
  if(a > b)
    return a;
  return b;
}

int32 max(int32 a, int32 b)
{
  if(a > b)
    return a;
  return b;
}

//the underscore is to differentiate it from math.f's "floor()"
real32 _floor(real32 x)
{
  i32 xi = (i32)x;
  return x < xi ? xi - 1 : xi;
}

//Vector structs/unions

struct v2
{
  r32 x,y;

  v2 operator +(v2 other)
  {
    v2 res;
    res.x = x + other.x;
    res.y = y + other.y;
    return res;
  }

  v2 operator -(v2 other)
  {
    v2 res;
    res.x = x - other.x;
    res.y = y - other.y;
    return res;
  }

  v2 operator *(v2 other)
  {
    v2 res;
    res.x = x * other.x;
    res.y = y * other.y;
    return res;
  }

  v2 operator /(v2 other)
  {
    v2 res;
    res.x = x / other.x;
    res.y = y / other.y;
    return res;
  }

  bool operator ==(v2 other)
  {
    return x == other.x && y == other.y;
  }
  
};

struct v3
{
  union
  {
    struct
    {
      r32 x,y,z;
    };
    struct
    {
      r32 r,g,b;
    };
  };

  v3 operator +(v3 other)
  {
    v3 res;
    res.x = x + other.x;
    res.y = y + other.y;
    res.z = z + other.z;
    return res;
  }

  v3 operator -(v3 other)
  {
    v3 res;
    res.x = x - other.x;
    res.y = y - other.y;
    res.z = z - other.z;
    return res;
  }

  v3 operator *(v3 other)
  {
    v3 res;
    res.x = x * other.x;
    res.y = y * other.y;
    res.z = z * other.z;
    return res;
  }

  v3 operator /(v3 other)
  {
    v3 res;
    res.x = x / other.x;
    res.y = y / other.y;
    res.z = z / other.z;
    return res;
  }

  bool operator ==(v3 other)
  {
    return x == other.x && y == other.y && z == other.z;
  }
};

struct v4
{
  union
  {
    struct
    {
      r32 x,y,z,w;
    };
    struct
    {
      r32 r,g,b,a;
    };
  };

  v4 operator +(v4 other)
  {
    v4 res;
    res.x = x + other.x;
    res.y = y + other.y;
    res.z = z + other.z;
    res.w = w + other.w;
    return res;
  }

  v4 operator -(v4 other)
  {
    v4 res;
    res.x = x - other.x;
    res.y = y - other.y;
    res.z = z - other.z;
    res.w = w - other.w;
    return res;
  }

  v4 operator *(v4 other)
  {
    v4 res;
    res.x = x * other.x;
    res.y = y * other.y;
    res.z = z * other.z;
    res.w = w * other.w;
    return res;
  }

  v4 operator /(v4 other)
  {
    v4 res;
    res.x = x / other.x;
    res.y = y / other.y;
    res.z = z / other.z;
    res.w = w / other.w;
    return res;
  }

  bool operator ==(v4 other)
  {
    return x == other.x && y == other.y && z == other.z
       && w == other.w;
  }
};

//int vectors

struct v2_i
{
  i32 x,y;

  v2_i operator +(v2_i other)
  {
    v2_i res;
    res.x = x + other.x;
    res.y = y + other.y;
    return res;
  }

  v2_i operator -(v2_i other)
  {
    v2_i res;
    res.x = x - other.x;
    res.y = y - other.y;
    return res;
  }

  v2_i operator *(v2_i other)
  {
    v2_i res;
    res.x = x * other.x;
    res.y = y * other.y;
    return res;
  }

  v2_i operator /(v2_i other)
  {
    v2_i res;
    res.x = x / other.x;
    res.y = y / other.y;
    return res;
  }

  bool operator ==(v2_i other)
  {
    return x == other.x && y == other.y;
  }
};

struct v3_i
{
  union
  {
    struct
    {
      i32 x,y,z;
    };
    struct
    {
      i32 r,g,b;
    };
  };

  v3_i operator +(v3_i other)
  {
    v3_i res;
    res.x = x + other.x;
    res.y = y + other.y;
    res.z = z + other.z;
    return res;
  }

  v3_i operator -(v3_i other)
  {
    v3_i res;
    res.x = x - other.x;
    res.y = y - other.y;
    res.z = z - other.z;
    return res;
  }

  v3_i operator *(v3_i other)
  {
    v3_i res;
    res.x = x * other.x;
    res.y = y * other.y;
    res.z = z * other.z;
    return res;
  }

  v3_i operator /(v3_i other)
  {
    v3_i res;
    res.x = x / other.x;
    res.y = y / other.y;
    res.z = z / other.z;
    return res;
  }

  bool operator ==(v3_i other)
  {
    return x == other.x && y == other.y && z == other.z;
  }
};

struct v4_i
{
  union
  {
    struct
    {
      i32 x,y,z,w;
    };
    struct
    {
      i32 r,g,b,a;
    };
  };

  v4_i operator +(v4_i other)
  {
    v4_i res;
    res.x = x + other.x;
    res.y = y + other.y;
    res.z = z + other.z;
    res.w = w + other.w;
    return res;
  }

  v4_i operator -(v4_i other)
  {
    v4_i res;
    res.x = x - other.x;
    res.y = y - other.y;
    res.z = z - other.z;
    res.w = w - other.w;
    return res;
  }

  v4_i operator *(v4_i other)
  {
    v4_i res;
    res.x = x * other.x;
    res.y = y * other.y;
    res.z = z * other.z;
    res.w = w * other.w;
    return res;
  }

  v4_i operator /(v4_i other)
  {
    v4_i res;
    res.x = x / other.x;
    res.y = y / other.y;
    res.z = z / other.z;
    res.w = w / other.w;
    return res;
  }

  bool operator ==(v4_i other)
  {
    return x == other.x && y == other.y && z == other.z
       && w == other.w;
  }

};

//Vector constructors

v2 _v2(r32 x, r32 y)
{
  v2 res;
  res.x = x;
  res.y = y;
  return res;
}

v2_i _v2(i32 x, i32 y)
{
  v2_i res;
  res.x = x;
  res.y = y;
  return res;
}

v3 _v3(r32 x, r32 y, r32 z)
{
  v3 res;
  res.x = x;
  res.y = y;
  res.z = z;
  return res;
}

v3_i _v3(i32 x, i32 y, i32 z)
{
  v3_i res;
  res.x = x;
  res.y = y;
  res.z = z;
  return res;
}

v4 _v4(r32 x, r32 y, r32 z, r32 w)
{
  v4 res;
  res.x = x;
  res.y = y;
  res.z = z;
  res.w = w;
  return res;
}

v4_i _v4(i32 x, i32 y, i32 z, i32 w)
{
  v4_i res;
  res.x = x;
  res.y = y;
  res.z = z;
  res.w = w;
  return res;
}

//Matrix math
//column major
/*

|------------|
|[0][2]  [1][2]  ...
|[0][1]  [1][1] ..
|[0][0]  [1][0] ...
|------------|
*/

struct mat3
{
  r32 cells[3][3];
};

struct mat4
{
  r32 cells[4][4];
};

//this ortho mat4 transforms a vector that is in the
// opengl [-1,1] space to:
//   - x:[left,right]
//   - y:[bottom,up]
// space
// mat4 ortho(r32 left, r32 right, r32 bottom, r32 up)
// {

// }
