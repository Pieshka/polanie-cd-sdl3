/////////////////////////////////////////////////////////////////////
//   image13h.cpp
//    kompilowac w modelu Large
////////////////////////////////////////////////////////////////////
//#include <stdio.h> // [PORT] Remove stdio.h
//#include <conio.h> // [PORT] Remove conio.h
//#include <malloc.h> // [PORT] Remove malloc.h
//#include <dos.h> // [PORT] Remove dos.h
//#include <mem.h> // [PORT] Remove mem.h
//#include <string.h> // [PORT] Remove string.h
#include "image13h.h"
#include "polanieapp.h" // [PORT] Add polanieapp.h
//////////////////////////////////////////////////////////////////
// Zmienne srodowiskowe
//////////////////////////////////////////////////////////////////
SDL_IOStream *palettefile,*graphicfile; // [PORT] Replace FILE with SDL_IOStream
char  *VirtualScreen;
char  *RealVirtualScreen;

char  *rgb=NULL,*Buffer330=NULL,*Rgb=NULL;
int length[91]={5,3,5,7,7,7,7,7,5,5,7,7,3,5,3,7,7,6,7,7,7,7,7,7,7,7,3,3,9,7,9,7,8,
        9,8,8,8,7,7,9,8,3,6,8,7,9,8,9,7,9,8,8,7,8,9,12,8,9,8,8,8,8,7,8,
        5,7,7,7,7,7,5,7,7,3,3,6,3,9,7,7,7,7,5,7,4,7,7,11,7,7,7};
char * _index[91]; // [PORT] Rename index to _index
#define index _index // [PORT] Define _index as index to avoid conflict on Linux
int lineLength=320;

int ClipX1=0,ClipX2=319,ClipY1=0,ClipY2=199;
///////////////////////////////////////////////////////////////////////
// inicjowanie ekranu virtualnego
///////////////////////////////////////////////////////////////////////
int InitVirtualScreen(void)
{
RealVirtualScreen=(char  *)SDL_malloc(64000); // [PORT] Replace malloc with SDL_malloc
if(RealVirtualScreen==NULL)return 0;
SDL_memset(RealVirtualScreen,0,64000); // [PORT] Replace memset with SDL_memset
return 1;
}
///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
void ClearScreen13h()
{
    SDL_memset(VirtualScreen,0,64000);              // [PORT] Replace memset with SDL_memset
}
//////////////////////////////////////////////////////////////////////
//kopiowanie virtualnego obrazu na ekran
//////////////////////////////////////////////////////////////////////
void ShowVirtualScreen(void)
{
if(RealVirtualScreen==NULL)return;
memcpy((void*)g_polanie->GetFrameBuffer(),(void*)RealVirtualScreen,63680); // [PORT] Replace 0xA0000 with g_polanie->GetFrameBuffer

}
///////////////////////////////////////////////////////////////////////////
// Zwalnianie pamieci zajmowanej przez ekran
//////////////////////////////////////////////////////////////////////
void FreeVirtualScreen(void)
{
if(RealVirtualScreen==NULL)return;
SDL_free(RealVirtualScreen); // [PORT] Replace free with SDL_free
RealVirtualScreen=NULL;
}
////////////////////////////////////////////////////////////////////////////
// ustawienie aktualnego ekranu dla funkcji graficznych
//   0-rzeczywisty       1-wirtualny
//////////////////////////////////////////////////////////////////////////
void SetScreen(int Screen)
{
if((Screen)&&(RealVirtualScreen!=NULL))VirtualScreen=RealVirtualScreen;
     else VirtualScreen=(char *)g_polanie->GetFrameBuffer(); // [PORT] Replace 0xA0000 with g_polanie->GetFrameBuffer
}
///////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void Init13h(void)
{/* [PORT] Remove unnecessary code
    union REGS r;

    r.h.ah = 0x0;
    r.h.al = 0x13;
    int386(0x10, &r, &r);*/
    SetScreen(0);
    SDL_memset((void*)g_polanie->GetFrameBuffer(),0,64000);              // [PORT] Replace 0xA0000 with g_polanie->GetFrameBuffer; Replace memset with SDL_memset
   
}
///////////////////////////////////////////////////////////
int InitBuffers13h()
{
    if((rgb=(char*)SDL_malloc(768))==NULL)return 1; // [PORT] Replace malloc with SDL_malloc
    if((Rgb=(char*)SDL_malloc(768))==NULL)return 1; // [PORT] Replace malloc with SDL_malloc
    if((Buffer330=(char  *)SDL_malloc(330))==NULL)return 1; // [PORT] Replace malloc with SDL_malloc
    return 0;
}
//////////////
void FreeBuffers13h()
{
    if(rgb!=NULL)SDL_free(rgb); // [PORT] Replace free with SDL_free
    if(Rgb!=NULL)SDL_free(Rgb); // [PORT] Replace free with SDL_free
    if(Buffer330!=NULL)SDL_free(Buffer330); // [PORT] Replace free with SDL_free
}

/////////////////////////////////////////////////////////////////
//END13H
///////////////////////////////////////////////////////////////
void Close13h(void)
{/* [PORT] Remove unnecessary code
    union REGS r;
    r.h.ah = 0x0;
    r.h.al = 0x03;
    int386(0x10, &r, &r);*/
}
///////////////////////////////////////////////////////////
//
//   SetClippingArea
//
///////////////////////////////////////////////////////////
void SetClippingArea13h(int x1, int y1, int x2,int y2)
{
    ClipX1=x1;
    ClipX2=x2;
    ClipY1=y1;
    ClipY2=y2;
}

///////////////////////////////////////////////////////////////////////
//  rysowanie punktu
///////////////////////////////////////////////////////////////////////
void PutPixel13h(int x,int y,int color)
{
    VirtualScreen[x+y*320]=(unsigned char )color;
}


//////////////////////////////////////////////////////////////////
// Load image from file "name" to picture
//////////////////////////////////////////////////////////////////
/* [PORT] Replace LoadImage13h with SDL friendly implementation
char* LoadImage13h(char*name)
{//return adres do obrazka lub NULL w przypadku wystapienia bledu
    FILE *file;
    char *picture;
    short int sizex,sizey;

    file=fopen(name,"rb");
    if(file==NULL)return NULL;
    fread(&sizex,2,1,file);
    fread(&sizey,2,1,file);
    picture=(char*)malloc(GetImageSize13h(0,0,sizex,sizey));
    if(picture==NULL){fclose(file);return NULL;}
    fseek(file,0,0);
    fread(picture,1,GetImageSize13h(0,0,sizex-1,sizey-1),file);
    fclose(file);
    return picture;
}*/
char* LoadImage13h(char*name)
{//return adres do obrazka lub NULL w przypadku wystapienia bledu
    SDL_IOStream *file;
    char *picture;
    short int sizex,sizey;

    file=SDL_IOFromFile(g_polanie->GetFilePath(name),"rb");
    if(file==NULL)return NULL;
    SDL_ReadIO(file,&sizex,2);
    SDL_ReadIO(file,&sizey,2);
    picture=(char*)SDL_malloc(GetImageSize13h(0,0,sizex,sizey));
    if(picture==NULL){SDL_CloseIO(file);return NULL;}
    SDL_SeekIO(file,0,SDL_IO_SEEK_SET);
    SDL_ReadIO(file,picture,GetImageSize13h(0,0,sizex-1,sizey-1));
    SDL_CloseIO(file);
    return picture;
}
      
////////////////////////////////////////////////////////////////////
int LoadToScreen13h(int offset,int line)
{
    int i=0,j=1;
    int Offset=offset*33000;
    short size;

    if(graphicfile==NULL)return 1;
         
    SDL_SeekIO(graphicfile,Offset,SDL_IO_SEEK_SET); // [PORT] Replace fseek with SDL_SeekIO
    SDL_ReadIO(graphicfile,&size,2); // [PORT] Replace fread with SDL_ReadIO
    SDL_ReadIO(graphicfile,&size,2); // [PORT] Replace fread with SDL_ReadIO
    SDL_ReadIO(graphicfile,&size,2); // [PORT] Replace fread with SDL_ReadIO
    if(line)j=0;
    for(i=0;i<99+j;i++)
    {
        size=SDL_ReadIO(graphicfile,(void*)(VirtualScreen+(line*320)+(i*320)),319); // [PORT] Replace fread with SDL_ReadIO
        if(size!=319)j=2;
    }    
    
    return j;
}

////////////////////////////////////////////////////////////////////////////
// Save picture image to file "name"
//////////////////////////////////////////////////////////////////////////
/* [PORT] Remove unnecessary function
 * int SaveImage256(char*name,char*picture)
{//return 0 if OK
    FILE *file;

    short int sizex,sizey,*buf=(short int*)picture;
    file=fopen(name,"wb");
    if(file==NULL)return 1;
    sizex=buf[0];
    sizey=buf[1];
    fwrite(picture,1,GetImageSize13h(0,0,sizex-1,sizey-1),file);
    fclose(file);
    return 0;
}*/

//////////////////////////////////////////////////////////////////
//  laduje bitmape pod adres map
//////////////////////////////////////////////////////////////////
void GetImage13h(int x1,int y1,int x2,int y2,char* picture)
{
    int i,j;
    short int *Buf=(short*)picture;
    
    
    Buf[0]=(short)(x2-x1);
    Buf[1]=(short)(y2-y1);    
    picture[4]=1;
    picture[5]=0;
    for(j=0;j<y2-y1;j++)
    {
       for(i=0;i<x2-x1;i++)picture[6+i+j*(x2-x1)]=VirtualScreen[i+x1+(j+y1)*320];
    }
}
////////////////////////////////////////////////////////////////////////
//
//       GetImageSize256
//
////////////////////////////////////////////////////////////////////////
int GetImageSize13h(int x1,int y1,int x2,int y2)
{
    return (x2-x1+1)*(y2-y1+1)+6;
}
//////////////////////////////////////////////////////////////////
//  wyswietla bitmape
//////////////////////////////////////////////////////////////////
void PutImage13h(int x,int y,char *picture,int how)
{
    int length,height,j,line,picturePos;
    short int *Buf=(short int *)picture;
    char color;
    if(x<0||y<0)return;
    if(!how)
    {
        length=Buf[0];
        height=Buf[1];
        if(y+height>199)height=199-y;
        line=(y*320)+x;//line poczatek linii na ekranie
        picturePos=6;
        for(j=0;j<height;j++)
        {
                SDL_memcpy((void*)&VirtualScreen[line],(void*)&picture[picturePos],length);          // [PORT] Replace memcpy with SDL_memcpy
                picturePos+=length;   
                line+=320;
        }
    }
    else
    {
        length=Buf[0];
        height=Buf[1];
        if(y+height>479)height=479-y;
        picturePos=6;
        line=(y)*320+x;//line poczatek linii na ekranie
        for(j=0;j<height;j++)
        {
                for(int i=0;i<length;i++)
                {
                         color=picture[picturePos];
                         picturePos++;       
                         if(color)
                         {
                             VirtualScreen[i+line]=color;
                         }
                }
                line+=320;
        }
    }    
}


//////////////////////////////////////////////////////////////////
//  wyswietla bitmape i zamienia kolor c1 na c2
//////////////////////////////////////////////////////////////////
void PutImageChange13h(int x,int y,char *picture,int how,char c1,char c2)
{
    int length,height,j,line,picturePos;
    short int *Buf=(short int *)picture;
    char color;
    
    length=Buf[0];
    height=Buf[1];
    if(y+height>479)height=479-y;
    picturePos=6;
    line=(y)*320+x;//line poczatek linii na ekranie
    for(j=0;j<height;j++)
    {
            for(int i=0;i<length;i++)
            {
                     color=picture[picturePos];
                     picturePos++;       
                     if(color||!how)
                     {
                         if(color==c1)
                         {
                             VirtualScreen[i+line]=c2;
                         }
                         else
                         {
                             VirtualScreen[i+line]=color;
                         }
                     }
            }
            line+=320;
    }
    
}




//////////////////////////////////////////////////////////////////
//  wyswietla bitmape inwersyjnie i zamienia kolor c1 na c2
//////////////////////////////////////////////////////////////////
void PutImageChangeInverse13h(int x,int y,char  *picture,int how,char c1,char c2)
{
int length,height,j,line,picturePos;
    short int *Buf=(short int *)picture;
    char color;
    
    length=Buf[0];
    height=Buf[1];
    if(y+height>479)height=479-y;
    picturePos=5;
    line=(y)*320+x;//line poczatek linii na ekranie
    for(j=0;j<height;j++)
    {
            picturePos+=length;        
            for(int i=0;i<length;i++)
            {
                     color=picture[picturePos];
                     picturePos--;       
                     if(color||!how)
                     {
                         if(color==c1)
                         {
                             VirtualScreen[i+line]=c2;
                         }
                         else
                         {
                            VirtualScreen[i+line]=color;
                         }
                     }
            }
            line+=320;
            picturePos+=length;        
    }
        
}

//////////////////////////////////////////////////////////////////
//  wyswietla kwadrat
//////////////////////////////////////////////////////////////////
void Rectangle13h(int x1,int y1,int x2,int y2, int color)
{
    int a;
    if(x1>x2)
    {
        a=x1;
        x1=x2;
        x2=a;
    }
    if(y1>y2)
    {
        a=y1;
        y1=y2;
        y2=a;
    } 
    int Y1=y1*lineLength;
    int Y2=y2*lineLength;
    int b=1,c=1,d=1; 
    a=1;
    

    if(x1>=ClipX2||x2<=ClipX1||y1>=ClipY2||y2<=ClipY1) return;
    

    
   
    if(x1<=ClipX1){x1=ClipX1+1;a=0;}
    if(x2>=ClipX2){x2=ClipX2-1;b=0;}
    if(y1<=ClipY1){y1=ClipY1+1;c=0;}
    if(y2>=ClipY2){y2=ClipY2-1;d=0;}

    //SetAreas(x1,y1,x2,y2);
    
    if(c)
    for(int i=x1; i<=x2; i++)
    {
        VirtualScreen[i+Y1]=(short)color;
     
    }
    
    if(d)
    for(int i=x1; i<=x2; i++)
    {
        VirtualScreen[i+Y2]=(short)color;
    }
    
    Y1=y1*lineLength+x1;
    Y2=y1*lineLength+x2;
    
    if(a)
    for(int i=y2-y1; i>0; i--)
    {
        VirtualScreen[Y1+=lineLength]=(short)color;
    }
    
    if(b)
    for(int i=y2-y1; i>0; i--)
    {
       VirtualScreen[Y2+=lineLength]=(short)color;
    }
}

//////////////////////////////////////////////////////////////////
//  wyswietla bar'a
//////////////////////////////////////////////////////////////////
void Bar13h(int x1,int y1,int x2,int y2, int color)
{
    int a;
    int length;
    if((x1==x2)||(y1==y2))return;
    if(x1>x2)
    {
        a=x1;
        x1=x2;
        x2=a;
    }
    if(y1>y2)
    {
        a=y1;
        y1=y2;
        y2=a;
    }
    if(x1>ClipX2||x2<ClipX1||y1>ClipY2||y2<ClipY1) return;
    if(x1<ClipX1)x1=ClipX1;
    if(x2>ClipX2)x2=ClipX2;
    if(y1<ClipY1)y1=ClipY1;
    if(y2>ClipY2)y2=ClipY2;
    if(x1>x2||y1>y2)return;
    if(x1>x2||y1>y2) return;
    
    length=x2-x1;
    for(int j=y1;j<y2;j++)
    {
        SDL_memset((void*)&VirtualScreen[x1+j*lineLength],color,length); // [PORT] Replace memset with SDL_memset
    }
}

//--------------------------------------------------------
//            InitText13h
//--------------------------------------------------------
int InitText13h()
{
unsigned int size;
int i,x1,x2;
char * literki;

literki=LoadImage13h("font.dat"); //??????????
if (literki==NULL) return 1;
PutImage13h(0,0,literki,0);
SDL_free(literki); // [PORT] Replace free with SDL_free

x1 = 7; x2 = 11;
for(i=0;i<33;i++)
 {x2 = x1+length[i]-1;
  size = GetImageSize13h(x1,8,x2,20);
  index[i] = (char*)SDL_malloc(size); // [PORT] Replace malloc with SDL_malloc
  if(index[i]!=NULL)  GetImage13h(x1,8,x2,20,index[i]);
     else return 1;
  x1 = x2;}

x1 = 8; x2 = 16;
for(i=33;i<64;i++)
 {x2 = x1+length[i]-1;
  size = GetImageSize13h(x1,32,x2,44);
  index[i] =(char*) SDL_malloc(size); // [PORT] Replace malloc with SDL_malloc
  if(index[i]!=NULL)  GetImage13h(x1,32,x2,44,index[i]);
     else return 1;
  x1 = x2;}

x1 = 7; x2 = 11;
for(i=64;i<91;i++)
 {x2 = x1+length[i]-1;
  size = GetImageSize13h(x1,56,x2,78);
  index[i] =(char*) SDL_malloc(size); // [PORT] Replace malloc with SDL_malloc
  if(index[i]!=NULL)  GetImage13h(x1,56,x2,78,index[i]);
     else return 1;
  x1 = x2;}

 return 0;
}

//------------------------------------------------------
//               Transform
//------------------------------------------------------
char Transform13h(unsigned char znak)
{
switch(znak)
     {case 134 : return '#';  //a
      case 145 : return '$'; //e
      case 162 : return '%';  //o'
      case 166 : return '&'; //z'
      case 167 : return 39 ; //z
      case 158 : return '*'; //s
      case 141 : return '+'; //c
      case 164 : return 47;  //n
      case 143 : return '<';  //A
      case 144 : return '=';  //E
      case 163 : return '>';  //O
      case 160 : return '@';  //Z'
      case 161 : return '['; //Z
      case 149 : return '\\';  //C
      case 152 : return ']';   //S
      case 156 : return '^';    //L
      case 165 : return '_';   //N
      case 146 : return '`';   //l
      }
  return znak;
}

//--------------------------------------------------------
//             ClearText
//-------------------------------------------------------
void ClearText13h()
{
int i;

for(i=0;i<92;i++) if(index[i]!=NULL) SDL_free(index[i]); // [PORT] Replace free with SDL_free
}
//--------------------------------------------------------
//             OutTextDelay13h                wypisywanie liter
//-------------------------------------------------------
void OutTextDelay13h(int x,int y,char *text,int colour1,int colour2,int del)
{
char * letter;
unsigned char znak;

while(*text!=NULL)
     {znak=Transform13h(*text);
     if(znak<32||znak>32+91)return;
     letter=(char*)index[znak-32];
     if(letter!=NULL)
     PutImageChange13h(x-1,y,letter,1,255,colour2);
     PutImageChange13h(x,y+1,letter,1,255,colour2);
     PutImageChange13h(x,y,letter,1,255,colour1);
     x=x+length[znak-32]-1;
     if(x>320)return;
     g_polanie->ProcessEvents(); SDL_Delay(del); // [PORT] Add event processing to show the effect; Replace delay with SDL_Delay
     text++;}
}
//--------------------------------------------------------
//             CenterText13h                wypisywanie liter
//-------------------------------------------------------
void CenterText13h(int xl,int yg,int xp,int yd,char *text,int colour)
{
int x,y,dl=0,wsk;
int i,ile=0;


wsk=SDL_strlen(text); // [PORT] Replace strlen with SDL_strlen
while(*text!=NULL)
{ile=ile+length[*text-32]-1;
if(ile>(xp-xl-12))*text=NULL;
text++; }
for(i=0;i<wsk;i++) text--;

wsk=SDL_strlen(text); // [PORT] Replace strlen with SDL_strlen
while(*text!=NULL)
{dl=dl+length[*text-32]-1;
text++;}
if(xp-xl<dl)return;
if(yd-yg<13)return;
y=(yd-yg+1)>>1;
y=yg+y-7;
x=(xp-xl-dl)>>1;
x=xl+x;
for(i=0;i<wsk;i++) text--;
OutText13h(x,y,text,colour);
}

//--------------------------------------------------------
//             OutText13h                wypisywanie liter
//-------------------------------------------------------
void OutText13h(int x,int y,char *text,int colour)
{
char * letter;
char znak;

while(*text!=NULL)
     {znak=Transform13h(*text);
     letter=index[znak-32];
     PutImageChange13h(x,y,letter,1,255,colour);
     x=x+length[znak-32]-1;
     text++;}
}
//--------------------------------------------------------
//             Write13h                wypisywanie liter
//-------------------------------------------------------

int Write13h(int x,int y, int maxx, int maxdl, char *txt, int tcolour,int bcolour) // [PORT] Add return type
{
  int cx=0,a,ll,xp,wsk=0,ile=0;
  char k,l;
  char str[2]={0,0};
wsk=SDL_strlen(txt); // [PORT] Replace strlen with SDL_strlen
while(*txt!=NULL)
{ile=ile+length[*txt-32]-1;
if(ile>maxx-12)*txt=NULL;
txt++; }
for(int i=0;i<wsk;i++) txt--;
cx=SDL_strlen(txt); // [PORT] Replace strlen with SDL_strlen
  do
  {
  if(maxx==0)
     Bar13h(x,y,x+(maxdl+1)*11-1,y+14,bcolour);
     else Bar13h(x,y,x+maxx,y+14,bcolour);
    xp=x;
    for(a=0;a<SDL_strlen(txt);a++) // [PORT] Replace strlen with SDL_strlen
       {str[0]=txt[a];
       if(a==cx)
         {Bar13h(xp,y+11,xp+length[txt[a]-32],y+12,tcolour);
          OutText13h(xp,y,str,tcolour);}
       else
          {OutText13h(xp,y,str,tcolour);}
       xp+=length[txt[a]-32]-1;}

    if(cx==SDL_strlen(txt)) // [PORT] Replace strlen with SDL_strlen
      {Bar13h(xp,y+11,xp+8,y+12,tcolour);}
    ll=SDL_strlen(txt); // [PORT] Replace strlen with SDL_strlen
    g_polanie->ProcessEvents(); k=mouse.Key;//k=getch(); // [PORT] Replace with funcionally equivalent code
    g_polanie->ProcessEvents(); if(!k)l=mouse.Key;//if(!k)l=getch(); // [PORT] Replace with funcionally equivalent code
    if(!k)
      switch(l)
           {case  75:if(cx>0)cx--;break;
            case  77:if(cx<ll)cx++;break;
        case  71:cx=0;break;
        case  79:cx=ll;break;
        case  83:if(cx<ll)
           for(a=cx;a<ll;a++)txt[a]=txt[a+1];break;}
    else
      switch(k)
           {case   8:if(cx>0)
                {cx--;
             for(a=cx;a<ll;a++)txt[a]=txt[a+1];}break;
    default:if((k>31)&&(ll<maxdl)&&(xp<maxx+x-18))
{if((k==' ')||(k>='0')&&(k<='z')&&((k<';')||(k>'@'))&&((k<'[')||(k>='a')))
  {for(a=ll+1;a>cx;a--)txt[a]=txt[a-1];
      txt[cx]=k;
      cx++;}break;}
      }
  }
  while((k!=13)&&(k!=27));
  return(k);
}
/////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
void OpenGraphicFile()
{
    graphicfile=SDL_IOFromFile(g_polanie->GetFilePath("graf.dat"),"rb"); // [PORT] Replace fopen with SDL_IOFromFile
}
/////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
void CloseGraphicFile()
{
    SDL_CloseIO(graphicfile); // [PORT] Replace close with SDL_CloseIO
}

/////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
void OpenPaletteFile()
{
    palettefile=SDL_IOFromFile(g_polanie->GetFilePath("pal.dat"),"rb"); // [PORT] Replace fopen with SDL_IOFromFile
}
/////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
void ClosePaletteFile()
{
    SDL_CloseIO(palettefile); // [PORT] Replace close with SDL_CloseIO
}


/////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
void LoadExtendedPalette(int pal)
{
        long whence=pal*768;
        if(rgb==NULL)return;
        if(palettefile==NULL)return;
        SDL_SeekIO(palettefile,whence,SDL_IO_SEEK_SET); // [PORT] Replace fseek with SDL_SeekIO
        SDL_ReadIO(palettefile,rgb,768); // [PORT] Replace fread with SDL_ReadIO
}

void LoadPalette13h(char* name)
{
        SDL_IOStream *f=SDL_IOFromFile(g_polanie->GetFilePath(name),"rb"); // [PORT] Replace FILE with SDL_IOStream; Replace fopen with SDL_IOFromFile
        if(f==NULL)return;
        if(rgb==NULL)return;
        if(palettefile==NULL)return;
        SDL_ReadIO(f,rgb,768); // [PORT] Replace fread with SDL_ReadIO
        SDL_CloseIO(f); // [PORT] Replace fclose with SDL_CloseIO
}

//////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////
void SetExtendedPalette()
{
if(rgb==NULL)return;
char* Palette=(char*)rgb;
/* This function sets the palette (will work on any VGA or SVGA card) by programming the card
        directly.  There IS a BIOS function to do it, but doing it directly is both quicker and - for protected
        mode programs - more straightforward   */

short int i;
char *p;//far

for (i=0; i<768; i++)
    Palette[i] = Palette[i] >> 2;

p = Palette;
/* [PORT] Remove unnecesary code
outp(0x3c6, 0xff);
for (i=0; i<=255; i++)
    {
    outp(0x3c8, i);
    outp(0x3c9, *p++);
    outp(0x3c9, *p++);
    outp(0x3c9, *p++);
    }*/
g_polanie->SetPalette(SDL_reinterpret_cast(Uint8 *, p)); g_polanie->ProcessEvents(); // [PORT] Replace it with functionally equivalent code

return;

}
/////////////////////////////////////////////////////////////////
void DownPalette(int speed)
{

/*
for(int i=1;i<32;i++)
{
   SetExtendedPalette();
}*/
BlackPalette();

}
//////////////////////////////////////////////////////////////////////
void BlackPalette(void)
{
    if(rgb==NULL)return;
    SDL_memset(rgb,0,768); // [PORT] Replace memset with SDL_memset
    SetExtendedPalette();
}
/////////////////////////////////////////////////////////////////
void RisePalette(int speed)
{

if(rgb==NULL){return;}
if(Rgb==NULL){SetExtendedPalette();return;}
// przepisanie rgb->Rgb i wyzerowanie rgb
/*
//  ---- rozjasnienie palety
for(int i=1;i<32;i++)
 {

 //SetExtendedPalette();
 
 }
*/


SetExtendedPalette();
}
//////////////////////////////////////////////////////////////////////
