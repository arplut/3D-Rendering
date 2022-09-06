#include<fstream>
#include<iostream>
using namespace std;
int main()
{
	ifstream testfile("testfile.txt");
	char c;
	int n1, n2, n3;
	int n4, n5, n6;
	while(testfile>>c>>n1>>n2>>n3)
	{
		if(c=='a')
			cout<<n1<<" "<<n2<<" "<<n3<<endl;//" "<<n4<<" "<<n5<<" "<<n6<<endl;
		else if(c=='b')
			cout<<n1<<" "<<n2<<" "<<n3<<endl;//" "<<n4<<" "<<n5<<" "<<n6<<endl;
	}
	testfile.close();
}
