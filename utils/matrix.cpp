// #include <types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct mat
{
  uint32 *vals;
  uint32 w;
  uint32 h;
};

void mat_print(mat *m)
{
  for(uint32 i=0;i<m->w*m->h;++i)
  {
    printf("%i ",m->vals[i]);

    if(i % m->w == m->w - 1)
      printf("\n");
  }
}

void mat_rotate_180(mat *m)
{
  size_t buf_size = sizeof(int32)*m->w*m->h;
  int32 *buf = (int32*)malloc(buf_size);

  for(uint32 i=0;i<m->w*m->h;++i)
  {
    buf[i] = m->vals[m->w*m->h - 1 - i];
  }

  memcpy(m->vals, buf, buf_size);
  free(buf);
}

//cw == true: rotates it clockwise
//cw == false: rotates it counter clockwise
void mat_rotate_90(mat *m, bool cw)
{
  size_t buf_size = sizeof(int32)*m->w*m->h;
  int32 *buf = (int32*)malloc(buf_size);

  for(uint32 i=0;i<m->w;++i)
  {
    for(uint32 j=0;j<m->h;++j)
    {
      int32 c_i = cw ? m->h - j - 1 : j;
      int32 new_buf_pos = j + m->h*i;
      int32 mat_pos;
      if(cw)
        mat_pos = m->w * c_i + i;
      else
        mat_pos = m->w * c_i + m->w - 1 - i;
      buf[new_buf_pos] = m->vals[mat_pos];
    }
  }

  uint32 temp = m->w;
  m->w = m->h;
  m->h = temp;

  memcpy(m->vals, buf, buf_size);
  free(buf);
}
