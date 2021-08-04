
int knr_style(int argc)
{
  if (argc == 0) {
    while (i < 10) {
      for (j = 0; j < 20; ++j) {
	i = i + 1;
      }
    }
  } else if (argc == 1) {
    j = j + 1;
  } else {
    k = k + 1;
  }

  if (argc == 0)
    i = i + 1;
  else if (argc == 2)
    j = j + 1;
  else
    k = k + 1;

  while (i < 10)
    k = k + 1;

  for (j = 0; j < 20; ++j)
    i = i + 1;
}

