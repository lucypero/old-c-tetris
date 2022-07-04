void gl_check_errors(const char *file, int line);

#ifdef DEBUG
#define GL_ERROR_CHECK() gl_check_errors(__FILE__, __LINE__)
#endif
#ifndef DEBUG
#define GL_ERROR_CHECK() 
#endif

void gl_check_errors(const char *file, int line)
{
  GLenum err;
  bool errors_found = false;
  while((err = glGetError()) != GL_NO_ERROR)
  {
    // Process/log the error.
    // TODO: display the error in hex
    errors_found = true;
    fprintf(stderr, "opengl error at %s:%i: %#010x\n", file, line, err);
  }
  if(errors_found)
    DBREAK();
}
