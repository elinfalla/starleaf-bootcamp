int strcmp(const char *s1, const char *s2)
{
  int count = 0;
  while(s1[count] != '\0')
  {
    if (s1[count] != s2[count])
      return (s1[count] - s2[count]);

    count++;
  }
  return s2[count]; //will return 0 if s2[count]='\0' aka success
}
