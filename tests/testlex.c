// program de testare a analizorului lexical, v1.1

int main()
{
	int i;
	i = 0;
	while (i < 10)
	{
		if (i / 2 == 1)
			put_i(i);
		i = i + 1;
	}
	if (4.9 == 49e-1 && 0.49E1 == 2.45 * 2.0)
		puts("yes");
	putc('#');
	puts(""); // pentru \n

	// double a= 2.e3;

	return 0;
}