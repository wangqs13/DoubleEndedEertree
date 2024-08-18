#include "DoubleEndedEertreeBySurfaceRecording.h"
#include <bits/stdc++.h>

using namespace std;

int main()
{
	DoubleEndedEertreeBySurfaceRecording<26> g;
	int q;
	cin >> q;
	while (q--)
	{
		int t;
		cin >> t;
		cin.ignore();
		if (t == 0) // push_front(c)
		{
			char c;
			cin >> c;
			g.push_front(c - 'a');
		}
		else if (t == 1) // push_back(c)
		{
			char c;
			cin >> c;
			g.push_back(c - 'a');
		}
		else if (t == 2) // pop_front
		{
			g.pop_front();
		}
		else // pop_back
		{
			g.pop_back();
		}
		// Output three numbers:
		// 1. the number of distinct palindromic substrings,
		// 2. the length of the longest palindromic prefix,
		// 3. the length of the longest palindromic suffix. 
		cout << g.count_palindrome() << " " << g.get_prepal()->len << " " << g.get_sufpal()->len << endl;
	}
	return 0;
}
