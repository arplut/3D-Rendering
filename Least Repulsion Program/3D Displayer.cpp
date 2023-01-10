#include <iostream>
#include<windows.h>
#include<graphics.h>
#include<math.h>
#include<conio.h>
#include<ctype.h>
#include<fstream>
#include<time.h>
#define pi 3.141592f
 
int fovp; //max screen coordinates
float fov; //max field of view
 
using namespace std;
 
int i;
bool gtest1;//for switching between showing outward/toward faces of solid object
bool gtest2(1);//for stopping execution of rendering window


struct cord
{
    float x,y,z;
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
 
int v=0,t=0;
 
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
        void addtriangle(int A, int B, int C, bool AB, bool BC, bool CA)
        {
            triangle::A=A;
            triangle::B=B;
            triangle::C=C;
            triangle::AB=AB;
            triangle::BC=BC;
            triangle::CA=CA;
            solid=0;
            ::t++;  
            calculate_midpoint();
        }
        void addtriangle(int A, int B, int C)
        {
            triangle::A=A;
            triangle::B=B;
            triangle::C=C;
            solid=1;
            AB=BC=CA=1;
            ::t++;  
        }
        void refreshtriangles(triangle T[])
        {
        	for(i=0;i<t;i++)
        	{
        		if(T[i].solid)
        		{
		        	T[i].calculate_midpoint();
		        	T[i].calculate_perpendicular();
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
*/
struct xycord
{	
	float x;
	float y;
};

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

class obj_details
{
	public:
		
}O[500];

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

 
int main()
{

	char ch;
	int axestemp[3]={t,t+1,t+2};
	V[v].addvertex(1,0,0); 
    V[v].addvertex(-1,0,0);    
    V[v].addvertex(0,1,0); 
    V[v].addvertex(0,-1,0);
    V[v].addvertex(0,0,1); 
    V[v].addvertex(0,0,-1);
	T[t].addtriangle(0+pv,1+pv,0+pv,1,0,0);
	T[t].addtriangle(2+pv,3+pv,2+pv,1,0,0); 
	T[t].addtriangle(4+pv,5+pv,4+pv,1,0,0);
	pv=v; 
	char type;
	float n1,n2,n3;
	ifstream file_("opcoo.txt");
	while(file_ >> type >> n1 >> n2 >> n3)
	{
		if(type=='v')
			V[v].addvertex(n1,n2,n3);
		else
			T[t].addtriangle(int(n1)-1+pv,int(n3)-1+pv,int(n2)-1+pv,1,0,0);
		//	T[t].addtriangle(int(n1)-1,int(n2)-1,int(n3)-1,1);
	}
	pv=v;
    T[0].refreshtriangles(T);
    
    cord ppos={-5,2.5,2};
    float pha=-pi/6; //angle of view in xy plane 
    float pva=-pi/8; //vertical factor
    calccampos(pha,pva);
    int maxx=1000,maxy=1000; //screen dimensions
    int rgb; //for shading
    int fn(40); //fov
    int poly[6]; //for solid fill
    
    int order[1200];
    for(i=0;i<t;i++)
    	T[i].calculate_distance(ppos);
	
	int pos;
	int j=0;
    int no_of_triangles_to_display(0);
    float max,pmax(100);
    
	gtest1=1;
	
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
        
        //drawing triangles
        cleardevice();
        for(i=0;i<t;i++)
        {
			if(T[i].solid && (V[T[i].A].disp_or_not && V[T[i].B].disp_or_not && V[T[i].C].disp_or_not)) 
			{
	            //shading
	            if(T[i].angle<0)
	                rgb=255-255*((-T[i].anglewithsun/(pi/2))*0.5f);
	            else
	                rgb=255-255*((T[i].anglewithsun/(pi/2))*0.5f);
		        setcolor((0x04000000 | RGB(rgb,rgb,rgb)));
		        setfillstyle(1,(0x04000000 | RGB(rgb,rgb,rgb)));
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
	            setcolor(WHITE);
	            if(i==axestemp[0])
	            	setcolor(BLUE);
	            else if(i==axestemp[1])
	            	setcolor(RED);
	            else if(i==axestemp[2])
	            	setcolor(GREEN);
	            if(T[i].AB && V[T[i].A].disp_or_not && V[T[i].B].disp_or_not)
	                line(maxx-V[T[i].A].hp,maxy-V[T[i].A].vp,maxx-V[T[i].B].hp,maxy-V[T[i].B].vp);
	            if(T[i].BC && V[T[i].B].disp_or_not && V[T[i].C].disp_or_not)
	                line(maxx-V[T[i].B].hp,maxy-V[T[i].B].vp,maxx-V[T[i].C].hp,maxy-V[T[i].C].vp);
	            if(T[i].CA && V[T[i].C].disp_or_not && V[T[i].A].disp_or_not)
	                line(maxx-V[T[i].C].hp,maxy-V[T[i].C].vp,maxx-V[T[i].A].hp,maxy-V[T[i].A].vp);
	        }
                
        }
        
        setcolor(YELLOW);
    	line(maxx+10,maxy,maxx-10,maxy);
    	line(maxx,maxy+10,maxx,maxy-10);
			
	
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
            case 'H':
            	if(pha<pi)
               		pha+=pi/180;
               	calccampos(pha,pva);
                break;
            case 'K':
            	if(pha>-pi)
                	pha-=pi/180;
                calccampos(pha,pva);
                break;
            case 'J':
            	if(pva>-pi/2)
                	pva-=pi/180;
                calccampos(pha,pva);
                break;
            case 'U':
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
            case 'Q':
            	if(gtest1)
                	gtest1=0;
                else
                	gtest1=1;
                break;
            case 'M':
            	gtest2=0;
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
       
    }while(gtest2);
    ShowWindow( hWnd, SW_SHOW ); 
	return 0;      
}
