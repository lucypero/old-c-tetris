//TODO: THIS ONLY WORKS ON LINUXXX
// WINDOWS LINE TERMINATORS WILL BREAK THIS
void move_to_next_line(char **str)
{
  char* it = *str;
  while(*it != '\n')
  {
    ++it;
  }
  *str = ++it;
}

//NOTE: returns all characters between
//        start_substr and end_substr
//        not including the line that start_substr
//        is in, nor end_substr
//        returns memory that must be freed by user
char* get_substr(const char* str,const char* start_substr,
    const char* end_substr)
{
  char *pos_start = (char*)strstr(str, start_substr);

  if(!pos_start)
  {
    fprintf(stderr, "pos_start not found in string\n");
    exit(1);
  }

  move_to_next_line(&pos_start);

  char *pos_end;

  if(end_substr != 0)
  {
    pos_end = (char*)strstr(pos_start, end_substr);
    if(!pos_end)
    {
      fprintf(stderr, "pos_end not found in string\n");
      exit(1);
    }
  }
  else
  {
    pos_end = pos_start + strlen(pos_start);
  }

  char *res_substr = (char*)malloc(pos_end - pos_start + 1);
  memcpy(res_substr, pos_start, pos_end - pos_start);
  res_substr[pos_end - pos_start] = 0;

  return res_substr;
}
