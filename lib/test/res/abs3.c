int test_abs(void)
{
  if (tc_abs(-5) != 5)
    return (0);

  else if (tc_abs(0) != 0)
    return(0);

  else
    return (1);
}

