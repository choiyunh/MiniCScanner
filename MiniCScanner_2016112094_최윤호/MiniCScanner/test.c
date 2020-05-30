/**
   A palindromic number is unchanged if its digits are reversed.
   121 or 1221 is a palindrome.
*/

const max = 100;

int sum(int n, int m)
{
	int i;

	write(n);
	write(m);
	i = n + m + max;
	if (i == 1) i = 100;
	return i;
}

void main()
{
	int x;
	char c = 'c';
	double d = 1.5;
	string s = "str";
	for (int i = 0; i < 5; i++) {
		if (x < 3)
			continue;
		d = 2.0;
		x++;
	}
	switch (c) {
	case 'c': c = 'b';
		break;
	default: c = 'c';
		break;
	}
	write(max);
	x = 333;
	x++;
	write(x);
	x = sum(10, 20);
	write(x);
}
