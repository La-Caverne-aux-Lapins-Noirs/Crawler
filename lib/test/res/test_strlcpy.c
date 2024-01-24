
#include	<stddef.h>

char		*efstrlcpy(char		*destination,
			   const char	*source,
			   int		length);

static void	zero(char		*dest,
		     int		length)
{
  int		i;

  i = 0;
  while (i < length)
    dest[i++] = 0;
}

static int	test_null_ptrs(void)
{
  if (efstrlcpy(NULL, "abc", 0) != NULL)
    return (2);
  if (efstrlcpy("abc", NULL, 0) != NULL)
    return (1);
  return (0);
}

static int	test_positive_strlcpy(void)
{
  char		buffer[8];
  char		*temporary;

  zero(buffer, sizeof(buffer));
  temporary = "abc";
  if (efstrlcpy(&buffer[0], temporary, 3) != &buffer[0])
    return (6);
  if (buffer[0] != temporary[0] || buffer[1] != temporary[1] || buffer[2] != '\0')
    return (5);
  buffer[0] = '_';
  if (efstrlcpy(&buffer[0], temporary, 0) != &buffer[0])
    return (4);
  if (buffer[0] != '_')
    return (3);
  if (efstrlcpy(&buffer[0], temporary, 4) != &buffer[0])
    return (2);
  if (buffer[0] != temporary[0] || buffer[1] != temporary[1] ||
      buffer[2] != temporary[2] || buffer[3] != temporary[3])
    return (1);
  return (0);
}

static int	test_negative_strlcpy(void)
{
  char		buffer[8];
  char		*temporary;

  zero(buffer, sizeof(buffer));
  temporary = "abc";
  if (efstrlcpy(&buffer[0], temporary, -3) != &buffer[0])
    return (4);
  if (buffer[0] != temporary[1] || buffer[1] != temporary[2] || buffer[2] != '\0')
    return (3);
  if (efstrlcpy(&buffer[0], temporary, -4) != &buffer[0])
    return (2);
  if (buffer[0] != temporary[0] || buffer[1] != temporary[1] ||
      buffer[2] != temporary[2] || buffer[3] != temporary[3])
    return (1);
  return (0);
}

int		test_strlcpy(void)
{
  int		result;

  if ((result = test_positive_strlcpy()) != 0)
    return (result + 20);
  if ((result = test_negative_strlcpy()) != 0)
    return (result + 10);
  return (test_null_ptrs());
}

