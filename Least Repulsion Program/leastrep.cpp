#include<iostream>
#include<math.h>
#include<conio.h>
#include<process.h>
#include<fstream>
#include<windows.h>
#define pi 3.14159
using namespace std;
struct cord
{
	float x; float y; float z;
   float ha; float va;
};


float anglecalc(cord lA, cord lB)
{
	float magA, magB;
	magA=lA.x*lA.x+lA.y*lA.y+lA.z*lA.z;
	magA=sqrt(magA);
	magB=lB.x*lB.x+lB.y*lB.y+lB.z*lB.z;
	magB=sqrt(magB);

	float thetha;
	thetha=lA.x*lB.x+lA.y*lB.y+lA.z*lB.z;
	thetha=thetha/(magA*magB);
	thetha=acos(thetha);

	if (thetha<0)
   		thetha=-thetha;
	return thetha;
}

float forcecalc(cord lA, cord lB)
{
	float r;
	r=sqrt((lA.x-lB.x)*(lA.x-lB.x)+(lA.y-lB.y)*(lA.y-lB.y)+(lA.z-lB.z)*(lA.z-lB.z));
	float force;
	force=/* k*q1*q2/ */ 1/(r*r);
	return force;
}

void cordcalc(float ha, float va, cord &l)
{
	cord ppos;
	float h, planedist;
	h=sin(va)*1;
	planedist=cos(va)*1;
	l.x=planedist*cos(ha);
	l.y=planedist*sin(ha);
	l.z=h;
}

float sumofforces(int pos, int n, cord L[],int disp=0)
{
	float sum(0);
	for(int i=0;i<n;i++)
	{
		if(i!=pos)
		{
			sum+=forcecalc(L[pos],L[i]);
			if(disp&&n<10)
				cout<<pos+1<<"-"<<i+1<<":"/*<<forcecalc(L[pos],L[i])<<" "*/<<int(anglecalc(L[pos],L[i])*180/pi)<<"\t";
		}
	}
	if(disp&&n<10)
		cout<<endl;
	return sum;
}

void displayall(int n, cord L[])
{
	for (int i=0;i<n;i++)
	{
		cout<<(int(L[i].x*100))/float(100)<<" "<<(int(L[i].y*100))/float(100)<<" "<<(int(L[i].z*100))/float(100);
      cout<<" "<<sqrt(L[i].x*L[i].x+L[i].y*L[i].y+L[i].z*L[i].z)<<endl;
	}

}

void whichdirection(int pos, int n, cord L[] ,int &stable)
{
	/*
		H  A  B
		G  x  C
		F  E  D
	*/
	float least(100);
	int cas(0);
	float deg1=pi/180;
	
	cordcalc(L[pos].ha,L[pos].va,L[pos]);
  	if(sumofforces(pos,n,L)<least)
	{
		least=sumofforces(pos,n,L);
		cas=0;
	}  
	//cout<<"X  0 "<<sumofforces(pos,n,L)<<endl;
	
	cordcalc(L[pos].ha+deg1,L[pos].va,L[pos]);
	if(sumofforces(pos,n,L)<least)
	{
		least=sumofforces(pos,n,L);
		cas=1;
	}
	//cout<<"C  1 "<<sumofforces(pos,n,L)<<endl;
	cordcalc(L[pos].ha-deg1,L[pos].va,L[pos]);
	if(sumofforces(pos,n,L)<least)
	{
		least=sumofforces(pos,n,L);
		cas=2;
	}
	//cout<<"G  2 "<<sumofforces(pos,n,L)<<endl;

	cordcalc(L[pos].ha,L[pos].va+deg1,L[pos]);
	if(sumofforces(pos,n,L)<least)
	{
		least=sumofforces(pos,n,L);
		cas=-1;
	}
	//cout<<"A -1 "<<sumofforces(pos,n,L)<<endl;

	cordcalc(L[pos].ha,L[pos].va-deg1,L[pos]);
	if(sumofforces(pos,n,L)<least)
	{
		least=sumofforces(pos,n,L);
		cas=-2;
	}
	//cout<<"E -2 "<<sumofforces(pos,n,L)<<endl;

	cordcalc(L[pos].ha+deg1,L[pos].va+deg1,L[pos]);
	if(sumofforces(pos,n,L)<least)
	{
		least=sumofforces(pos,n,L);
		cas=3;
	}
	//cout<<"B  3 "<<sumofforces(pos,n,L)<<endl;

	cordcalc(L[pos].ha+deg1,L[pos].va-deg1,L[pos]);
	if(sumofforces(pos,n,L)<least)
	{
		least=sumofforces(pos,n,L);
		cas=4;
	}
	//cout<<"D  4 "<<sumofforces(pos,n,L)<<endl;

	cordcalc(L[pos].ha-deg1,L[pos].va+deg1,L[pos]);
	if(sumofforces(pos,n,L)<least)
	{
		least=sumofforces(pos,n,L);
		cas=5;
	}
	//cout<<"H  5 "<<sumofforces(pos,n,L)<<endl;

	cordcalc(L[pos].ha-deg1,L[pos].va-deg1,L[pos]);
	if(sumofforces(pos,n,L)<least)
	{
		least=sumofforces(pos,n,L);
		cas=6;
	}
	//cout<<"F  6 "<<sumofforces(pos,n,L)<<endl;

	//cout<<"Initial angles: "<<L[pos].ha*180/pi<<" "<<L[pos].va*180/pi<<endl;
	if(pos==0)
	{
		cas=0;
	}
	if(cas!=0)
	{
		stable=0;
	}
	switch(cas)
	{
		case 0:
			L[pos].ha=L[pos].ha;
			L[pos].va=L[pos].va;
			break;
		case 1:
			L[pos].ha=L[pos].ha+deg1;
			L[pos].va=L[pos].va;
			break;
		case 2:
			L[pos].ha=L[pos].ha-deg1;
			L[pos].va=L[pos].va;
			break;
		case -1:
			L[pos].ha=L[pos].ha;
			L[pos].va=L[pos].va+deg1;
			break;
		case -2:
			L[pos].ha=L[pos].ha;
			L[pos].va=L[pos].va-deg1;
			break;
		case 5:
			L[pos].ha=L[pos].ha-deg1;
			L[pos].va=L[pos].va+deg1;
			break;
		case 6:
			L[pos].ha=L[pos].ha-deg1;
			L[pos].va=L[pos].va-deg1;
			break;
		case 4:
			L[pos].ha=L[pos].ha+deg1;
			L[pos].va=L[pos].va-deg1;
			break;
		case 3:
      		L[pos].ha=L[pos].ha+deg1;
			L[pos].va=L[pos].va+deg1;
			break;
	}
	cordcalc(L[pos].ha,L[pos].va,L[pos]);
	//cout<<"Case: "<<cas<<" New angles: "<<L[pos].ha*180/pi<<" "<<L[pos].va*180/pi<<endl;
	//cout<<"least: "<<least<<" "<<sumofforces(pos,n,L,1)<<endl<<endl;
	sumofforces(pos,n,L,1);
}


int main()
{
	cord L[25];

	
	int i,sign;
	int N;
	cout<<"Enter number of bonds: ";
	cin>>N;
	
	L[0].ha=0;
	L[0].va=pi/2;
	L[0].x=L[0].y=0;
	L[0].z=1;

	for(i=1,sign=1;i<N;i++,sign*=-1)
	{
		L[i].ha=sign*(i)*pi/18;
		L[i].va=0;
		sign*=-1;
		cordcalc(L[i].ha,L[i].va,L[i]);
	}
	

	int test=1,gctest=1;
	char ch;
	int stable=0;
	while(test)
	{
      	system("CLS");
      	if(N<10)
      		cout<<"ANGLES: "<<endl;
      	stable=1;
		for(i=0;i<N;i++)
			whichdirection(i,N,L,stable);
		if(stable==1)
		{
			gctest=1;
		}
		if(N<10)	
			cout<<endl;
		cout<<"COORDINATES: "<<endl;
	  	displayall(N,L);

	  	cout<<endl<<"Press S to skip to end | E to view config in 3D | Any key to run 1 iteration";
	  	if(gctest==1)
	  		ch=getch();
	  	else
	  		ch='Q';
	  	if(toupper(ch)=='S')
	  	{
	  		gctest=0;	
		}
		
		if(toupper(ch)=='E')
		{
			fstream f;
			f.open("opcoo.txt",ios::out);
			f<<'v'<<' '<<0<<' '<<0<<' '<<0<<endl;
			for(i=0;i<N;i++)
			{
				f<<'v'<<' '<<L[i].x/2<<' '<<L[i].y/2<<' '<<L[i].z/2<<endl;
				f<<'f'<<' '<<1<<' '<<2<<' '<<i+2<<endl;
			}
			f.close();
			ShellExecute( NULL, NULL, "3D Displayer.exe", NULL, NULL, SW_SHOW );
			break;
		}
	}
}
