#include<iostream>
#include<windows.h>
#include<graphics.h>
#include<math.h>
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
bool gtest3(0);//for showing lines of triangles
int i,j;
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
        void addtriangle(int A, int B, int C, float x, float y, float z, bool perpend)
        {
            triangle::A=A;
            triangle::B=B;
            triangle::C=C;
            solid=1;
            AB=BC=CA=1;
            tri_p.x=x;
            tri_p.y=y;
            tri_p.z=z;
            if(perpend)
            	calculate_perpendicular();
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
            if(gtest1)
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

int objc(0);
rgbcolor whit={255,255,255};
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
		rgbcolor findcolor(obj_details O[500],int sei)
		{
			objc=0;
			while(objc<=o)
			{
				if(i>=O[objc].lower && i<O[objc].upper)
					return O[objc].OC;
				objc++;
			}	
			return whit;
		}		
}O[500];

class labels
{
	public:
		int vertexno;
		char label_text[30];
		void addlabel(char textinput[],int lx)
		{
			vertexno=lx;
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

int maxx=1000,maxy=1000; //screen dimensions

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
				else if(type[0]=='f'&& type[1]==0)
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
					settextstyle(3,0,4);
					char str[50]="Error | Select file again";
					outtextxy(maxx-300,maxy,str);
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
		    	{
					cleardevice();
					settextstyle(3,0,4);
					char str[100]="PRESS Y TO CONVERT AND CREATE A NEW FILE OF .p3dm FILE FORMAT";
					outtextxy(maxx/2-75,maxy-50,str);
				}
		    	char yornochoice;
		    	yornochoice=getch();
		    	if(toupper(yornochoice)=='Y')
		    		createconvertedfile();
		    	else
		    	{
					cleardevice();
					settextstyle(3,0,4);
					char str[50]="ERROR IN FILE TYPE. SELECT AGAIN";
					outtextxy(maxx-300,maxy,str);
					return 1;
				}
		    		
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
				{
					cleardevice();
					settextstyle(3,0,4);
					char str[50]="ERROR IN FILE TYPE. SELECT AGAIN";
					outtextxy(maxx-300,maxy,str);
				}
				return 1;
			}
			char type[50];
			float n1,n2,n3,n4,n5,n6;
			ifstream file_(file_location);
			while(file_ >> type >> n1 >> n2 >> n3 >> n4 >> n5 >> n6)
			{
				if(type[0]=='v')
					V[v].addvertex(n1,n2,n3);
				else if(type[0]=='t' && n4==n5 && n5==n6 && n6==0)
				{
					if(type[1]=='l')
						T[t].addtriangle(int(n1)-1+pv,int(n3)-1+pv,int(n2)-1+pv,n4,n5,n6,0,1,0);
					else
						T[t].addtriangle(int(n1)-1+pv,int(n3)-1+pv,int(n2)-1+pv,n4,n5,n6,1);
				}
				else if(type[0]=='t')
				{
					T[t].addtriangle(int(n1)-1+pv,int(n3)-1+pv,int(n2)-1+pv,n4,n5,n6,0);
				}	
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

class menu
{
	public:
		void displaymenu()
		{
			
			char str[40];
			
			
			settextstyle(3,0,7);
			strcpy(str,"3D RENDERING SOFTWARE");
			outtextxy(maxx/6,maxy-250,str);
			
			settextstyle(3,0,4);
			strcpy(str,"BY PRATUL VENKATESH");
			outtextxy(maxx/6,maxy-170,str);
			
			strcpy(str,"MENU");
			outtextxy(maxx/6,maxy-90,str);
			strcpy(str,"OPEN FILE");
			outtextxy(maxx/6+30,maxy,str);
			strcpy(str,"INSTRUCTIONS");
			outtextxy(maxx/6+30,maxy+90,str);
			strcpy(str,"EXIT PROGRAM");
			outtextxy(maxx/6+30,maxy+180,str);
			
			setfillstyle(0,BLACK);
			rectangle(maxx/6,maxy-15,maxx/6+325,maxy+50);
			rectangle(maxx/6,maxy+75,maxx/6+325,maxy+140);
			rectangle(maxx/6,maxy+165,maxx/6+325,maxy+230);
		}
		int comparemouseclick (int m_x, int m_y)
		{
			if(m_x>maxx/6 && m_x<maxx/6+325 && m_y>maxy-15 && m_y<maxy+50)
				return 1;
			else if(m_x>maxx/6 && m_x<maxx/6+325 && m_y>maxy+75 && m_y<maxy+140)
				return 2;
			else if(m_x>maxx/6 && m_x<maxx/6+325 && m_y>maxy+165 && m_y<maxy+230)
				return 3;
			else
				return 0;	
		}
		void displayinstructions()
		{
			ifstream f("Instructions.txt");
			
			int lspacing=maxy/10+50;
			char str[80];
			settextstyle(3,0,5);
			f.getline(str,80,'\n');
			outtextxy(maxx/6,lspacing,str);
			
			lspacing+=40;
			settextstyle(3,0,3);
			while(f.getline(str,80,'\n'))
			{
				outtextxy(maxx/6,lspacing,str);
				lspacing+=30;
			}
		}
}M;

int main()
{
	
/*	cout<<"Save File?";
	getch();
	F.savefile();*/
	
	
	bool menu(1);
	char ch;
	cord ppos={-5,5,1.5};
    float pha=-pi/4; //angle of view in xy plane 
    float pva=0; //vertical factor
    calccampos(pha,pva);
    
    rgbcolor rgb; //for shading
    int fn(40); //fov
    int poly[8]; //for solid fill
    char* label_text;
    int shading_factor;
    
    int order[1200];
    
	
	int pos;
	j=0;
    int no_of_triangles_to_display(0);
    float max,pmax(100);
    
    int m_x, m_y;//mouse click x and y
	gtest1=1;
	
	bool mouse_continuity(0);
	
    initwindow(maxx,maxy,"3D Viewer");
    
    //to maximize window
	HWND hWnd = ::GetActiveWindow();
    ShowWindow( hWnd, SW_MAXIMIZE );
    
    do
    {
    	fov=fn*pi/180;
	    maxx=getmaxx();
	    maxx/=2;
	    maxy=getmaxy();
	    maxy/=2;
	    
    	if(menu)
    	{
		    int menuchoice(0);
		    cleardevice();
		    M.displaymenu();
		    int m_x(0),m_y(0);
		    
		    while(!(menuchoice>=1 && menuchoice<=3))
		    {
			    if (ismouseclick(WM_LBUTTONDOWN))	
					getmouseclick(WM_LBUTTONDOWN, m_x, m_y);	
				if (ismouseclick(WM_LBUTTONUP))	
					clearmouseclick(WM_LBUTTONUP);
				menuchoice=M.comparemouseclick(m_x,m_y);
				delay(50);
		    }
			if(menuchoice==1)
			{
				/*1. Open File*/
				F.selectfile();
				while(F.importfile())
				{
					F.selectfile();	
				}
				for(i=0;i<t;i++)
    				T[i].calculate_distance(ppos);
				menu=0;
			}
			else if(menuchoice==2)
			{
				cleardevice();
				M.displayinstructions(); 
				getch();
				menuchoice=0;
			}
			else if(menuchoice==3)
			{
				return 0;
			}
			
		}
		else
		{
	        
	        /*
	        //camera rotation
	        
	        ppos.x=ppos.x*cos(pi/180)+ppos.y*sin(pi/180);
	        ppos.y=-ppos.x*sin(pi/180)+ppos.y*cos(pi/180);
	        pha-=pi/180;
	        
	        */
	        if(maxx>maxy)
	            fovp=maxx;
	        else
	            fovp=maxy;
	        
	        
	        for(i=0;i<v;i++)
	        {
	            V[i].calc_hp_vp(ppos,pha,pva);  
	        }
	        
	             
	        //sorting
	        j=0;
	        pmax=100;
	        no_of_triangles_to_display=0;
	        while(j<t)
	        {
	        	max=0;
	        	for(i=0;i<t;i++)
	        		if(T[i].distance>max && T[i].distance<pmax)
	        		{
	        			max=T[i].distance;
	        			pos=i;
	        		}
	        	pmax=max;
	        	if(T[pos].visibility(ppos)==1 || !T[pos].solid)
	        	{
		        	order[no_of_triangles_to_display]=pos;
		        	no_of_triangles_to_display++;
		        }
		        j++;	
	        }
	        
	        
	        cleardevice();
	        
	        //drawing triangles
	        for(j=0;j<no_of_triangles_to_display;j++)
	        {
	        	i=order[j];
	        	rgb=O[0].findcolor(O,i);
				if(T[i].solid && (V[T[i].A].disp_or_not && V[T[i].B].disp_or_not && V[T[i].C].disp_or_not)) 
				{
		            //shading
		            if(T[i].angle<0)
		                shading_factor=255-255*((-T[i].anglewithsun/(pi/2))*0.5f);
		            else
		                shading_factor=255-255*((T[i].anglewithsun/(pi/2))*0.5f);
		            rgb.R=rgb.R*shading_factor/255;
		            rgb.G=rgb.G*shading_factor/255;
		            rgb.B=rgb.B*shading_factor/255;
			        setcolor((0x04000000 | RGB(rgb.R,rgb.G,rgb.B)));
			        setfillstyle(1,(0x04000000 | RGB(rgb.R,rgb.G,rgb.B)));
		            poly[0]=maxx-V[T[i].A].hp;
		            poly[1]=maxy-V[T[i].A].vp;
		            poly[2]=maxx-V[T[i].B].hp;
		            poly[3]=maxy-V[T[i].B].vp;
		            poly[4]=maxx-V[T[i].C].hp;
		            poly[5]=maxy-V[T[i].C].vp;
		            fillpoly(3,poly);
	            }     
	            else
	            {
					//lines 
		            setcolor((0x04000000 | RGB(rgb.R,rgb.G,rgb.B)));
			        setfillstyle(1,(0x04000000 | RGB(rgb.R,rgb.G,rgb.B)));
		            if(T[i].AB && V[T[i].A].disp_or_not && V[T[i].B].disp_or_not)
		                line(maxx-V[T[i].A].hp,maxy-V[T[i].A].vp,maxx-V[T[i].B].hp,maxy-V[T[i].B].vp);
		            if(T[i].BC && V[T[i].B].disp_or_not && V[T[i].C].disp_or_not)
		                line(maxx-V[T[i].B].hp,maxy-V[T[i].B].vp,maxx-V[T[i].C].hp,maxy-V[T[i].C].vp);
		            if(T[i].CA && V[T[i].C].disp_or_not && V[T[i].A].disp_or_not)
		                line(maxx-V[T[i].C].hp,maxy-V[T[i].C].vp,maxx-V[T[i].A].hp,maxy-V[T[i].A].vp);
		        }
		        if(gtest3 && T[i].solid)
	            {
					//lines 
		            setcolor(WHITE);
		            if(T[i].AB && V[T[i].A].disp_or_not && V[T[i].B].disp_or_not)
		                line(maxx-V[T[i].A].hp,maxy-V[T[i].A].vp,maxx-V[T[i].B].hp,maxy-V[T[i].B].vp);
		            if(T[i].BC && V[T[i].B].disp_or_not && V[T[i].C].disp_or_not)
		                line(maxx-V[T[i].B].hp,maxy-V[T[i].B].vp,maxx-V[T[i].C].hp,maxy-V[T[i].C].vp);
		            if(T[i].CA && V[T[i].C].disp_or_not && V[T[i].A].disp_or_not)
		                line(maxx-V[T[i].C].hp,maxy-V[T[i].C].vp,maxx-V[T[i].A].hp,maxy-V[T[i].A].vp);
		        }
	                
	        }
	        
	        //label rednering
	        for(j=0;j<l;j++)
	        {
	        	if(V[L[j].vertexno].disp_or_not)
	        	{
		        	label_text=L[j].label_text;
		        	settextstyle(3,HORIZ_DIR,2);
		        	setcolor(WHITE);
					outtextxy(maxx-V[L[j].vertexno].hp,maxy-V[L[j].vertexno].vp,label_text);
					setcolor(YELLOW);
					setfillstyle(1,YELLOW);
					poly[0]=maxx-V[L[j].vertexno].hp-6;
					poly[1]=maxy-V[L[j].vertexno].vp+8;
					
					poly[2]=maxx-V[L[j].vertexno].hp-6;
					poly[3]=maxy-V[L[j].vertexno].vp+14;
					
					poly[4]=maxx-V[L[j].vertexno].hp-12;
					poly[5]=maxy-V[L[j].vertexno].vp+14;
					
					poly[6]=maxx-V[L[j].vertexno].hp-12;
					poly[7]=maxy-V[L[j].vertexno].vp+8;
		        	fillpoly(4,poly);
		        }
			}
	        
			setcolor(YELLOW);
	    	line(maxx+10,maxy,maxx-10,maxy);
	    	line(maxx,maxy+10,maxx,maxy-10);
			
			{
				settextstyle(3,0,1);
				char str[30]="Press H for HELP";
				outtextxy(20,maxy*2-30,str);
			}
			delay(10);	
		
			ch=getch();
	        switch (toupper(ch))
	        {
	            case 'W':
	                orient_motion(pha,ppos,ch);
	                break;
	            case 'S':
	                orient_motion(pha,ppos,ch);
	                break;
	            case 'A':
	                orient_motion(pha,ppos,ch);
	                break;
	            case 'D':
	                orient_motion(pha,ppos,ch);
	                break;
	            case 'E':
	                ppos.z=ppos.z+0.1;
	                break;
	            case 'F':
	                ppos.z=ppos.z-0.1;
	                break;
	            case 'J':
	            	if(pha<pi)
	               		pha+=pi/180;
	               	calccampos(pha,pva);
	                break;
	            case 'L':
	            	if(pha>-pi)
	                	pha-=pi/180;
	                calccampos(pha,pva);
	                break;
	            case 'K':
	            	if(pva>-pi/2)
	                	pva-=pi/180;
	                calccampos(pha,pva);
	                break;
	            case 'I':
	            	if(pva<pi/2)
	                	pva+=pi/180;
	                calccampos(pha,pva);
	                break;
	            case 'G':
	                if(fn<55)
	                    fn+=1;
	                break;
	            case 'R':
	                if(fn>29)
	                    fn-=1;
	                break;
	            case 'H':
	            	setcolor(BLACK);
					setfillstyle(1,BLACK);
					poly[0]=maxx/6-40;
					poly[1]=maxy/10+30;
					
					poly[2]=maxx/6-40;
					poly[3]=maxy+200;
					
					poly[4]=maxx/6+510;
					poly[5]=maxy+200;
					
					poly[6]=maxx/6+510;
					poly[7]=maxy/10+30;
		        	fillpoly(4,poly);
					setcolor(YELLOW);
	            	M.displayinstructions(); 
					getch();
	                break;
	            case 'Q':
	            	if(gtest1)
	                	gtest1=0;
	                else
	                	gtest1=1;
	                break;
	            case 'M':
	            	return 0;
	            	break;
	            case 'B':
	            	if(gtest3)
	                	gtest3=0;
	                else
	                	gtest3=1;
	                break;
	            default:
	                break;
	        }
	     	orient_motion(pha,ppos,ch);
	        //excess horizontal orienation correction
	        if(pha>pi)
	            pha=-2*pi+pha;
	        else if(pha==-pi || pha<-pi)
	            pha=2*pi+pha;
				//break;
       }
    }while(gtest2);
    ShowWindow( hWnd, SW_SHOW ); 
	
	getch();
    return 0;
}

