#include<fstream>
#include<iostream>
#include<stdio.h>
using namespace std;
int main()
{
	fstream f;
	rename("testfile1.obj","testfile1.txt");
	f.open("testfile1.txt",ios::in);
	char s;
	f>>s;
	cout<<s;
	f.close();
	rename("testfile1.txt","testfile1.obj");
	
}
