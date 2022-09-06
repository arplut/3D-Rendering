#include <fstream>
#include<iostream>
#include<windows.h>
//#include<graphics.h>
#include<math.h>
#include<conio.h>
#include<ctype.h>
#include<fstream>
#include<time.h>
#define pi 3.141592f

using namespace std;

//global variables
int fovp; //max screen coordinates
float fov; //max field of view
bool gtest1;//for switching between showing outward/toward faces of solid object
bool gtest2(1);//for stopping execution of rendering window
int i;
int v=0,t=0,o=0,l=0;

struct cord
{
    float x,y,z;
};

struct xycord
{	
	float x;
	float y;
};

struct rgbcolor
{
	unsigned int R,G,B;	
};


cord campos;//camera position

void calccampos(float pha, float pva)
{
	float dist;
	dist=cos(pva)*1;
	campos.x=cos(pha)*dist;
	campos.y=sin(pha)*dist;
	campos.z=sin(pva)*1;
}
 

 
class vertex
{
    public:
        cord point;
        int hp,vp; //coordinates of projection of point on screen
        bool disp_or_not;
        void addvertex(float x, float y, float z)
        {
            point.x=x;
            point.y=y;
            point.z=z;
            v++;
        }
        void calc_hp_vp(cord ppos, float pha, float pva)
        {
            float fhp,fvp;
			float ha;
            disp_or_not=1;
             
            //calculation of relative position
            cord spoint;
            spoint.x=point.x-ppos.x;
            spoint.y=point.y-ppos.y;
            spoint.z=point.z-ppos.z;

            //angles 0 and 90
            bool do_ang_calc=1;
            if(spoint.y==0&&spoint.x<0)
            {
                ha=pi;
                do_ang_calc=0;
            }   
            else if(spoint.y==0&&spoint.x>0)
            {
                ha=0;
                do_ang_calc=0;
            }
            else if(spoint.x==0&&spoint.y>0)
            {
                ha=pi/2;
                do_ang_calc=0;
            }
            else if(spoint.x==0&&spoint.y<0)
            {
                ha=-pi/2;
                do_ang_calc=0;
            }
             
            //angle calculation
            if(do_ang_calc)
            {
                ha=atan(spoint.y/spoint.x);
                if(ha<0)
                    ha=-ha;
                 
                if(spoint.x<0 && spoint.y<0)
                    ha=-pi+ha;
                else if(spoint.x>0 && spoint.y<0)
                    ha=-ha;
                else if(spoint.x<0 && spoint.y>0)
                    ha=pi-ha;
            }
             
            if(ha<-pi)
                ha=2*pi+ha;
 
            //relative horizontal angle accounting for quadrants
            if(pha>0)
            {
                if(ha>0)
                    ha=ha-pha;  
                else
                {
                    if(ha<(pha-pi))
                        ha=2*pi-pha+ha;
                    else
                        ha=-pha+ha; 
                }   
            }
            else
            {
                if(ha<0)
                    ha=ha-pha;      
                else
                {
                    if(ha>(pha+pi))
                        ha=-2*pi-pha+ha;
                    else
                        ha=ha-pha;  
                }       
            }
            
            
            
            float magA,magB,angle;
            magA=1;
			//camera radius
            magB=spoint.x*spoint.x+spoint.y*spoint.y+spoint.z*spoint.z;
            magB=sqrt(magB);
             
            //cosine of angle
            angle=(campos.x*spoint.x+campos.y*spoint.y+campos.z*spoint.z)/(magA*magB);
            //angle is acos of above equation
            angle=acos(angle);
            
            bool skip(1);
            if(!(angle>=-pi/2 && angle<=pi/2))//for checking if object is behind camera
            {
            	disp_or_not=hp=vp=0;
            	skip=0;
			}
           	
           	if(skip)
           	{
	           	float t2;//refer rotationpolar
	           	t2=spoint.x*spoint.x+spoint.y*spoint.y;
	           	t2=sqrt(t2);
	           	t2=cos(ha)*t2;
	           	
	           	float ae;//angle of elevation
	           	ae=atan(spoint.z/t2);
	           	
	           	fvp=tan(ae-pva)*0.1;
	           	float hypo;//hypotenuse of triangle formed by hp and vp
	           	hypo=tan(angle)*0.1;
	           	if(hypo<0)	
	           		hypo=-hypo;
	           	fhp=hypo*hypo-fvp*fvp;
	           	fhp=sqrt(fhp);
	           	if(ha<0)
	           		fhp=-fhp;
	     		if(!(fvp>=-0.2246 && fvp<=0.2246) || !(fhp>=-0.2246 && fhp<=0.2246))
	                disp_or_not=0; 
	    
	            fhp=fhp*fovp/(tan(fov)*0.1);
	            fvp=fvp*fovp/(tan(fov)*0.1);
	            hp=fhp;
	            vp=fvp;
            }
        }
}V[1200];
 
class triangle
{
 
    public:
        int A,B,C;  //index values reffering to an element of vertex array
        bool AB,BC,CA; //whether line should be displayed or not
        cord tri_p,midp; //perpendicular to plane of triangle and midpoint of triangle
        bool solid; //whether triangle belongs to solid object or not
        float angle, anglewithsun;
        float distance;
        triangle()
        {
            AB=BC=CA=1;
        }
        void addtriangle(int A, int B, int C, float x, float y, float z,bool AB, bool BC, bool CA)
        {
            triangle::A=A;
            triangle::B=B;
            triangle::C=C;
            triangle::AB=AB;
            triangle::BC=BC;
            triangle::CA=CA;
            solid=0;
            tri_p.x=x;
            tri_p.y=y;
            tri_p.z=z;
            ::t++;  
            calculate_midpoint();
        }
        void addtriangle(int A, int B, int C, float x, float y, float z)
        {
            triangle::A=A;
            triangle::B=B;
            triangle::C=C;
            solid=1;
            AB=BC=CA=1;
            tri_p.x=x;
            tri_p.y=y;
            tri_p.z=z;
            ::t++;  
        }
        void refreshtriangles(triangle T[])
        {
        	for(i=0;i<t;i++)
        	{
        		if(T[i].solid)
        		{
		        	T[i].calculate_midpoint();
		        //	T[i].calculate_perpendicular();
					T[i].calculate_anglewithsun();
				}
			}	
		}
        void calculate_midpoint()
        {
        	midp.x=(V[A].point.x+V[B].point.x+V[C].point.x)/3;
            midp.y=(V[A].point.y+V[B].point.y+V[C].point.y)/3;
            midp.z=(V[A].point.z+V[B].point.z+V[C].point.z)/3;
		}
        void calculate_perpendicular()
        {
            //perpendicular calculator using cross product AB AC
            cord relB, relC;
             
            //relative position of B
            relB.x=V[B].point.x-V[A].point.x;
            relB.y=V[B].point.y-V[A].point.y;
            relB.z=V[B].point.z-V[A].point.z;
             
            //relative position of C    
            relC.x=V[C].point.x-V[A].point.x;
            relC.y=V[C].point.y-V[A].point.y;
            relC.z=V[C].point.z-V[A].point.z;
             
            tri_p.x=relB.y*relC.z-relC.y*relB.z;
            tri_p.y=-relB.x*relC.z+relC.x*relB.z;
            tri_p.z=relB.x*relC.y-relC.x*relB.y;
         
        }
        bool visibility(cord ppos)
        {
            cord line;
            //A=line joining ppos(camera) to midpoint of triangle
            line.x=ppos.x-midp.x;
            line.y=ppos.y-midp.y;
            line.z=ppos.z-midp.z;
            //tri_p is the other line whose center is already relative to midpoint of triangle
             
            //calculation of angle using dot product
            float magA,magB;
            magA=line.x*line.x+line.y*line.y+line.z*line.z;
            magA=pow(magA,0.5);
            magB=tri_p.x*tri_p.x+tri_p.y*tri_p.y+tri_p.z*tri_p.z;
            magB=pow(magB,0.5);
             
            //cosine of angle
            angle=(line.x*tri_p.x+line.y*tri_p.y+line.z*tri_p.z)/(magA*magB);
             
            //angle is acos of above equation
            angle=acos(angle);
     
            if(gtest1)
				angle=pi-angle;
            if(angle<(pi/2) && angle>(-pi/2))
                return 1;
            else
                return 0;
        }
        void calculate_anglewithsun()
        {
        	cord line;
            //A=line joining sun to midpoint of triangle
            line.x=1;
            line.y=3;
            line.z=2;
            //tri_p is the other line whose center is already relative to midpoint of triangle
             
            //calculation of angle using dot product
            float magA,magB;
            magA=line.x*line.x+line.y*line.y+line.z*line.z;
            magA=pow(magA,0.5);
            magB=tri_p.x*tri_p.x+tri_p.y*tri_p.y+tri_p.z*tri_p.z;
            magB=pow(magB,0.5);
             
            //cosine of angle
            anglewithsun=(line.x*tri_p.x+line.y*tri_p.y+line.z*tri_p.z)/(magA*magB);
             
            //angle is acos of above equation
            anglewithsun=acos(anglewithsun);
            if(!gtest1)
				anglewithsun=pi-anglewithsun;
		}
        void calculate_distance(cord ppos)
        {
        	//calculation of relative position
            cord spoint;
            spoint.x=midp.x-ppos.x;
            spoint.y=midp.y-ppos.y;
            spoint.z=midp.z-ppos.z;
			
			distance=spoint.x*spoint.x+spoint.y*spoint.y+spoint.z*spoint.z;
			distance=sqrt(distance);
		}
}T[1200];

int pv(0);//pv is the no. of existing vertices before new set of trangles are added
/*
class obj_functions
{
	public:
		void shiftx(cord &point, float x)
		{
			point.x+=x;	
		}
		void shifty(cord &point, float y)
		{
			point.y+=y;	
		}
		void shiftz(cord &point, float z)
		{
			point.z+=z;	
		}
		void rotationz(float thetha, cord &point, float ox, float oy)
		{
			point.x-=ox;
			point.y-=oy;
			
			float temp;
			thetha=-thetha;
			temp=point.x*cos(thetha)+point.y*sin(thetha);
			point.y=-point.x*sin(thetha)+point.y*cos(thetha);
			point.x=temp;
			
			point.x+=ox;
			point.y+=oy;
		}
		void rotationy(float thetha, cord &point, float ox,float oz)
		{
			point.x-=ox;
			point.z-=oz;
			
			float temp;
			temp=point.x*cos(thetha)-point.z*sin(thetha);
			point.z=point.x*sin(thetha)+point.z*cos(thetha);
			point.x=temp;
			
			point.x+=ox;
			point.z+=oz;
		}
		void rotationx(float thetha, cord &point, float oy, float oz)
		{
			point.y-=oy;
			point.z-=oz;
			
			float temp;
			temp=point.y*cos(thetha)+point.z*sin(thetha);
			point.z=-point.y*sin(thetha)+point.z*cos(thetha);
			point.y=temp;
			
			point.y+=oy;
			point.z+=oz;
		}
		void rotationpolar(float pha,float thetha, cord &spoint, float ox, float oy, float oz)
		{
			spoint.x-=ox;
			spoint.y-=oy;
			spoint.z-=oz;
			rotationz(pha,spoint,0,0);
			
			float ha;
			//angles 0 and 90
		    bool do_ang_calc=1;
		    if(spoint.y==0&&spoint.x<0)
		    {
		        ha=pi;
		        do_ang_calc=0;
		    }   
		    else if(spoint.y==0&&spoint.x>0)
		    {
		        ha=0;
		        do_ang_calc=0;
		    }
		    else if(spoint.x==0&&spoint.y>0)
		    {
		        ha=pi/2;
		        do_ang_calc=0;
		    }
		    else if(spoint.x==0&&spoint.y<0)
		    {
		        ha=-pi/2;
		        do_ang_calc=0;
		    }
		     
		    //angle calculation
		    if(do_ang_calc)
		    {
		        ha=atan(spoint.y/spoint.x);
		        if(ha<0)
		            ha=-ha;
		         
		        if(spoint.x<0 && spoint.y<0)
		            ha=-pi+ha;
		        else if(spoint.x>0 && spoint.y<0)
		            ha=-ha;
		        else if(spoint.x<0 && spoint.y>0)
		            ha=pi-ha;
		    }
		     
		    if(ha<-pi)
		        ha=2*pi+ha;
		
		    //relative horizontal angle accounting for quadrants
		    if(pha>0)
		    {
		        if(ha>0)
		            ha=ha-pha;  
		        else
		        {
		            if(ha<(pha-pi))
		                ha=2*pi-pha+ha;
		            else
		                ha=-pha+ha; 
		        }   
		    }
		    else
		    {
		        if(ha<0)
		            ha=ha-pha;      
		        else
		        {
		            if(ha>(pha+pi))
		                ha=-2*pi-pha+ha;
		            else
		                ha=ha-pha;  
		        }       
		    }
		    
			float d_xy;//xy plane projection
			d_xy=spoint.x*spoint.x+spoint.y*spoint.y;
			d_xy=sqrt(d_xy);
			
			float t1,t2;//sides of triangle formed by center, d_xy and perpendicular from pha to d_xy
			t1=sin(ha)*d_xy;
			t2=cos(ha)*d_xy;
		
			float d_rad;//radius of circle along which point is rotating
			d_rad=spoint.z*spoint.z+t2*t2;
			d_rad=sqrt(d_rad);
			
			float ang;//angle between point and xy projection along circle of rotation
			ang=atan(spoint.z/t2);
			
			float new_d_xy, new_z;
			new_d_xy=d_rad*cos(ang+thetha);
			new_z=d_rad*sin(ang+thetha);
			
			spoint.z=new_z;
			
			float x1,y1,x2,y2;
			x1=cos(pha)*new_d_xy;
			y1=sin(pha)*new_d_xy;
			
			if(ha>=0)
			{
				x2=cos(pha+pi/2)*t1;
				y2=sin(pha+pi/2)*t1;
			}
			else
			{
				x2=cos(pha-pi/2)*-t1;
				y2=sin(pha-pi/2)*-t1;	
			}
			
			spoint.x=x1+x2;
			spoint.y=y1+y2;
			
			spoint.x+=ox;
			spoint.y+=oy;
			spoint.z+=oz;
		}
}O_F;

class obj_builder
{
	public:
		void addpolygon(int n,float d, float extrusion=0,bool s_or_r=1) 
		{
			float r;
			float thetha;
			thetha=2*pi/n;
			if(s_or_r==1)
			{
				r=d;
			}
			else
			{
				r=d*cos((pi/2-thetha)/2)+sin((pi/2-thetha)/2)*d/tan(thetha);
			}
			xycord xyvertices[30];
			for(i=0;i<n;i++)
			{
				xyvertices[i].x=r*cos(thetha*i);
				xyvertices[i].y=r*sin(thetha*i);
			}
			V[v].addvertex(0,0,0);
			if(extrusion==0)
			{
				for(i=0;i<n;i++)
				{
					V[v].addvertex(xyvertices[i].x,xyvertices[i].y,0);
					if(i<n-1)
						T[t].addtriangle(pv+i+2,pv+i+1,pv);
				}
				T[t].addtriangle(pv+1,pv+i,pv);	
			}
		}
		 	
}O_B;
*/
class obj_details
{
	public:
		int lower,upper;
		rgbcolor OC;
		void addobject(int l, int u, int r, int g, int b)
		{
			lower=l;
			upper=u;
			OC.R=r;
			OC.G=g;
			OC.B=b;
			o++;
		}		
}O[500];

class labels
{
	public:
		int vertexno;
		char label_text[30];
		void addlabel(char textinput[],int lx)
		{
			vertexno=v-1;
			for(i=0;i<strlen(textinput);i++)
				textinput[i]=textinput[i+1];
			strcpy(label_text,textinput);
			l++;	
		}
}L[100];

void orient_motion(float pha, cord &ppos, char ch)
{
    int negate(-1);
    if(toupper(ch)=='W'||toupper(ch)=='S')
    {
        if(toupper(ch)=='W')
            negate*=-1;
        if(pha>=0 && pha<=(pi/2))
        {
            ppos.x+=0.1*cos(pha)*negate;
            ppos.y+=0.1*sin(pha)*negate;
        }
        else if(pha>(pi/2) && pha<=pi)
        {
             
            ppos.x-=0.1*sin(pha-pi/2)*negate;
            ppos.y+=0.1*cos(pha-pi/2)*negate;   
        }
        else if(pha<0 && pha>=(-pi/2))
        {
            ppos.x+=0.1*cos(-pha)*negate;
            ppos.y-=0.1*sin(-pha)*negate;   
        }
        else if(pha<(-pi/2) && pha>(-pi))
        {
            ppos.x-=0.1*sin(-pha-pi/2)*negate;
            ppos.y-=0.1*cos(-pha-pi/2)*negate;  
        }
    }
    else if(toupper(ch)=='A'||toupper(ch)=='D')
    {
        if(toupper(ch)=='A')
            negate*=-1;
        if(pha>=0 && pha<=(pi/2))
        {
            ppos.x-=0.1*sin(pha)*negate;
            ppos.y+=0.1*cos(pha)*negate;
        }
        else if(pha>(pi/2) && pha<=pi)
        {
            ppos.x-=0.1*cos(pha-pi/2)*negate;
            ppos.y-=0.1*sin(pha-pi/2)*negate;   
        }
        else if(pha<0 && pha>=(-pi/2))
        {
            ppos.x+=0.1*sin(-pha)*negate;
            ppos.y+=0.1*cos(-pha)*negate;   
        }
        else if(pha<(-pi/2) && pha>-pi)
        {
            ppos.x+=0.1*cos(-pha-pi/2)*negate;
            ppos.y-=0.1*sin(-pha-pi/2)*negate;  
        }
    }
    for(i=0;i<t;i++)
    	T[i].calculate_distance(ppos);
         
}

class filehandling
{
	char file_location[180];
	public:
		//function to convert obj/txt file to new .p3dm file
		void createconvertedfile()
		{
			fstream imp_,exp_;
			imp_.open(file_location,ios::in);
			int slen;
		    slen=strlen(file_location);
		    file_location[slen-1]='t';
		    file_location[slen-2]='a';
		    file_location[slen-3]='d';
		    exp_.open(file_location,ios::out);
		    char type[50];
			float n1,n2,n3,n4,n5,n6;
		    while(imp_ >> type)
			{
				if(type[0]=='v' && type[1]==0)
				{
					imp_>>n1>>n2>>n3;
					exp_<<'v'<<' '<<n1<<' '<<n2<<' '<<n3<<" 0 0 0"<<'\n';
				}	
				else if(type[0]=='t'&& type[1]==0)
				{
					imp_>>n1>>n2>>n3;
					exp_<<'t'<<' '<<n1<<' '<<n2<<' '<<n3<<" 0 0 0"<<'\n'; 
				}
				//	T[t].addtriangle(int(n1)-1,int(n2)-1,int(n3)-1,1);
			}
		    imp_.close();
		    exp_.close();
		}
		//function to put address of selected file into text file "Command.txt"
		void selectfile()
		{
			fstream cmd;
			cmd.open("Command.txt",ios::out);
			cmd<<"%";
			cmd.close();
		    bool flag1(0);
		    
			ShellExecute( NULL, NULL, "Select File.exe", NULL, NULL, SW_SHOW );
		    ifstream location_;
			while(!flag1)
		    {	
		    	
				location_.open("Command.txt");
				file_location[0]=0;
				location_.getline(file_location,180);
				//junk eliminator
				int j;
				for(i=0;i<strlen(file_location);i++)
					if(file_location[i]==':')
					{
						for(j=i-1;j<=strlen(file_location);j++)
							file_location[j-i+1]=file_location[j];
						break;
					}
				flag1=1;
				if(file_location[0]=='%')
				{
					Sleep(200);
					flag1=0;
				}	
				location_.close();	
		    }
		    //cout<<"Press Y to convert .obj file format to .3dp file format if file is .obj format else press N to continue"<<endl;
		    //convertfile();
		}
		
		int importfile()
		{
			
			bool flag(0);
			while(!flag)
			{
				flag=1;
				if(file_location[0]=='%')
				{
					cout<<"Error in finding selected file. Select file again"<<endl;
					flag=0;
					selectfile();
				}
			}
			//identification of .p3dm file
			char fextension[5];
		    for(i=0;i<strlen(file_location);i++)
		    {
		    	if(file_location[i]=='.')
		    	{
		    		for(int j=i+1;j<=strlen(file_location);j++)
		    		{
		    			fextension[j-i-1]=file_location[j];
		    		}
		    	}
		    }
		    if(strcmpi("obj",fextension)==0||strcmpi("txt",fextension)==0)
		    {
		    	cout<<"Press Y to convert and create new file format of .p3dm format"<<endl;
		    	char yornochoice;
		    	yornochoice=getch();
		    	if(toupper(yornochoice)=='Y')
		    		createconvertedfile();
			}
			else if(strcmpi("p3dm",fextension)==0)
			{
				char datfileloc[180];
		    	strcpy(datfileloc,file_location);
		    	int slend;
		    	slend=strlen(datfileloc);
		    	datfileloc[slend-1]=0;
		    	datfileloc[slend-2]='t';
		   		datfileloc[slend-3]='a';
		    	datfileloc[slend-4]='d';
		    	rename(file_location,datfileloc);
		    	file_location[slend-1]=0;
		    	file_location[slend-2]='t';
		    	file_location[slend-3]='a';
		    	file_location[slend-4]='d';
			}
			else
			{
				cout<<"Error in file type. Select again"<<endl;
				return 1;
			}
			char type[50];
			float n1,n2,n3,n4,n5,n6;
			ifstream file_(file_location);
			while(file_ >> type >> n1 >> n2 >> n3 >> n4 >> n5 >> n6)
			{
				if(type[0]=='v')
					V[v].addvertex(n1,n2,n3);
				else if(type[0]=='t')
					T[t].addtriangle(int(n1)-1+pv,int(n3)-1+pv,int(n2)-1+pv,n4,n5,n6);
				else if(type[0]=='o' )
					O[o].addobject(n1,n2,n3,n4,n5);
				else if(type[0]=='l')
					L[l].addlabel(type,n1);
				//	T[t].addtriangle(int(n1)-1,int(n2)-1,int(n3)-1,1);
			}
		    T[0].refreshtriangles(T);
		    file_.close();
		    char p3dmfileloc[180];
		    strcpy(p3dmfileloc,file_location);
		    int slen;
		    slen=strlen(p3dmfileloc);
		    p3dmfileloc[slen+1]=0;
		    p3dmfileloc[slen]='m';
		    p3dmfileloc[slen-1]='d';
		    p3dmfileloc[slen-2]='3';
		    p3dmfileloc[slen-3]='p';
		    rename(file_location,p3dmfileloc);
		    return 0;
		}
		void savefile()
		{
			fstream cmd;
			cmd.open("Command.txt",ios::out);
			cmd<<"%";
			cmd.close();
		    bool flag1(0);
		    
			ShellExecute( NULL, NULL, "Save File.exe", NULL, NULL, SW_SHOW );
		    
			ifstream location_;
		    char p3dmfileloc[180],datfileloc[180];
			while(!flag1)
		    {	
		    	
				location_.open("Command.txt");
				p3dmfileloc[0]=0;
				location_.getline(p3dmfileloc,180);
				//junk eliminator
				int j;
				for(i=0;i<strlen(p3dmfileloc);i++)
					if(p3dmfileloc[i]==':')
					{
						for(j=i-1;j<=strlen(p3dmfileloc);j++)
							p3dmfileloc[j-i+1]=p3dmfileloc[j];
						break;
					}
				flag1=1;
				if(p3dmfileloc[0]=='%')
				{
					Sleep(200);
					flag1=0;
				}	
				location_.close();	
		    }
			
			strcpy(datfileloc,p3dmfileloc);
			datfileloc[strlen(datfileloc)-4]=0;
			strcat(datfileloc,"dat");
			
			ofstream exp_(datfileloc);
			for(i=0;i<v;i++)
				exp_<<'v'<<' '<<V[i].point.x<<' '<<V[i].point.y<<' '<<V[i].point.z<<" 0 0 0"<<'\n';
			for(i=0;i<t;i++)
				exp_<<'t'<<' '<<T[i].A+1<<' '<<T[i].C+1<<' '<<T[i].B+1<<' '<<T[i].tri_p.x<<' '<<T[i].tri_p.y<<' '<<T[i].tri_p.z<<'\n';
			for(i=0;i<o;i++)
				exp_<<'o'<<' '<<O[i].lower<<' '<<O[i].upper<<' '<<O[i].OC.R<<' '<<O[i].OC.G<<' '<<O[i].OC.B<<' '<<0<<' '<<'\n';
			char labeltemp[30];
			for(i=0;i<l;i++)
			{
				labeltemp[0]='l';
				labeltemp[1]=0;
				strcat(labeltemp,L[i].label_text);
				int vno=L[i].vertexno+1;
				exp_<<labeltemp<<' '<<L[i].vertexno+1<<" 0 0 0 0 0"<<'\n';
			}
			exp_.close();
			remove(p3dmfileloc);
			rename(datfileloc,p3dmfileloc);	
		}
}F;

int main()
{
	int menuchoice;
	/*1. Open File*/
	F.selectfile();
	while(F.importfile())
	{
		F.selectfile();	
	}
	cout<<"Save File?";
	getch();
	F.savefile();
	
	/*Instructions
	W- Move Forward
	A- Move Left
	S- Move Back
	D- Move Right
	
	E- Move Up
	F- Move Down
	
	R- Zoom In
	G- Zoom Out
	
	U- Look Up
	J- Look Down
	K- Look Right
	H- Look Left
	
	Q- Invert Objects
	B- Show Triangles
	*/
	
	
	
	
	
	
	
	getch();
    return 0;
}

