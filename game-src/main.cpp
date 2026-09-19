#include "shar.h"
//////////////////////////////////////////
//
//    Modul glowny
/////////////////////////////////////////
//#include <stdio.h> // [PORT] Remove stdio.h
//#include <conio.h> // [PORT] Remove conio.h
//#include <dos.h> // [PORT] Remove dos.h
//#include <malloc.h> // [PORT] Remove malloc.h
//#include <graph.h> // [PORT] Remove graph.h
#include "mouse.h"
#include "mover.h"
#include "image13h.h"
//#include "menegdma.h" // [PORT] Remove menegdma.h
//#include <process.h> // [PORT] Remove process.h
//#include "playfli.h" // [PORT] Remove playfli.h
//#include "cd.h" // [PORT] Remove cd.h
#include "sound.h" // [PORT] Add sound.h
#include "polanieapp.h" // [PORT] Add polanie.h

//class MENEGERDMA SND(184,0x1000); // [Port] Replace MENEGERDMA with Sound
class Sound SND; // [Port] Replace MENEGERDMA with Sound
PolanieApp *g_polanie; // [PORT] Define g_polanie
//========zmienne=====================
extern int diff;
int   endGame=0;
int   level;
char  *picture[MaxPictures],*missiles[6][3][3],*tlo,*Mysz[13];
char  *movers[5][13][3][3],*ramka[4];   //faza:typ:dx:dy
char  *buttons[16];
char  *dead[3],*Hit[2];
char  *shadow;
char  *face[16];//0-9 twarze, 10-15 budynki
char  *Buttons[4];  //moze byc [8] bo save i smenu to te same przyciski
char drive[4]="\0\0\0"; // [PORT] replace drive from d:\\ to zero
//======zmienne extern===========
extern int licznik;
extern char prowintion[25];
extern char prowintionInit[25];
//extern char hasla[10];

//=======Deklaracje funkcji=============

void MainMenuDispatchEvent(void);
void PressButton(int B,int P);
void MouseEngine(void);
void NewGame(void);
//====== Funkcje extern ======================
extern void InitPicture(void);
extern void FreeMemory(void);
extern void ShowMainMenu(void);
extern void InitBattle(void);
extern void Battle(int);
extern int LoadGame();
extern void ShowSubMenu(void);
extern void ShowIntro(void);
extern void ShowPicture(int,int);
extern void ShowText(int,int);
extern void Haslo(char*,char);
extern int GetMemory(void);
//////////////////////////////////
//////////////////////////////////////////
extern int ladowanie(void);

int IsFile(char*);
////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////
int show;
int main() // [PORT] Main must return int
{
g_polanie = new PolanieApp(); g_polanie->Init(0); SND.Init(); // [PORT] Add PolanieApp
if(InitBuffers13h()){cprintf("BLAD !!!\n\rBrak pamieci operacyjnej. \n\rProgram wymaga 4MB RAM\n"); return 1;} // [PORT] Main must return int
//_clearscreen(_GCLEARSCREEN); // [PORT] Remove conio remnants
//_setbkcolor(4); // [PORT] Remove conio remnants
//_settextcolor(14); // [PORT] Remove conio remnants
cprintf("                                P O L A N I E                   \n\r");
//_setbkcolor(0); // [PORT] Remove conio remnants
#ifdef SHAREWARE
cprintf("                        SHAREWARE                1996                          \n\n\r");
#else
cprintf("                        ver. 4.27               1997                          \n\n\r");
#endif
SDL_Delay(500); // [PORT] replace delay with SDL_Delay
//cprintf(" Wersja do wylacznego uzytku Ryszarda Cieslika szefa spolki CBS - Elektronik\n\r\n\r");
//cprintf("                       Rozpowszechnianie zabronione!\n\r");

/* [PORT] SETUP.INI is not necessary anymore
FILE* f=fopen("setup.INI","rb");
if(!f)
{
    printf("Blad odczytu pliku SETUP.INI.\n Uruchom program SETUP.EXE \n");
    return 1; // [PORT] Main must return int
}
fseek(f,0,0);
fread(drive,1,1,f);
if(drive[0])
{
    BigOnCDAudio();
    OnCDAudio();
}
else
{
    BigOffCDAudio();
    OffCDAudio();
}
fseek(f,7,0);
fread(drive,1,1,f);
fclose(f);
printf("CD drive : %s\n",drive);

FILE *f;
_settextcolor(7);*/
char ss[30];
/*sprintf(ss,"%sgraf.dat",drive);
//printf("Odczyt pliku %s \n",ss);
do
{
    f=fopen(ss,"rb");
    if(f==NULL)
    {
        printf("Brak dysku POLANIE.CD w napedzie %s \n Wloz dysk i nacisnij Enter lub Esc jezeli chesz zrezygnowac\n",drive);
        int key=getch();
        if(key==27)return 1; // [PORT] Main must return int
    }
}while(f==NULL);
fclose(f);
[PORT] SETUP.INI is not necessary anymore */
//if(!mouse.MouseInit()){cprintf("BLAD !!!\n\r----------- Brak sterownika myszy.---------");return;} // [PORT] Remove mouse initialization
//------------------------------------------
//if(ladowanie()){cprintf("BLAD !!!\n\r----------- Brak pliku SETUP.INI.  Uruchom program SETUP.EXE.-----------\n"); return; } // [PORT] Remove ladowani
//----------------------------------------------------------------------------
/*** AZ DO NASTEPNYCH GWIAZDEK JEST TO KOD EKSPERYMENTALNY - NALEZY GO USUNAC *** ---
sprintf(ss,"%sdata\\i001.dat",drive);
if(IsFile(ss))
{
   SND.PlayWav(ss);
   getch();
   SND.EndPlayWav();
}

sprintf(ss,"%sdata\\i002.dat",drive);
if(IsFile(ss))
{
   SND.PlayWav(ss);
   getch();
   SND.EndPlayWav();
}

sprintf(ss,"%sdata\\i003.dat",drive);
if(IsFile(ss))
{
   SND.PlayWav(ss);
   getch();
   SND.EndPlayWav();
}

//return;
// *** KONIEC KODU EKSPERYMENTALNEGO ***/
//--------------------------------------------

cprintf("\n\rAlokacja pamieci niskiej - grafika..................................  \n\r");
if(GetMemory()){cprintf("BLAD !!!\n\rBrak pamieci operacyjnej. \n\rProgram wymaga 586kB RAM\n"); return 1;} // [PORT] Main must return int

cprintf("Alokacja ekranu wirtualnego......................  \n\r");
if(!InitVirtualScreen())
{
    cprintf("BLAD !!!\n\rBrak pamieci operacyjnej. Program wymaga 4 MB RAM\n");
    return 1; // [PORT] Main must return int
}

cprintf("Inicjacja czytnika CD ROM........\n\r"); /* [PORT] Replace CD Audio with Sound
if(InitCD())
{
    do
    {
        cprintf("BLAD! Naped CD ROM nie gotowy.\n\r Wloz dysk POLANIE do stacji CD ROM i nacisnij ENTER \n\rEsc - anuluj");
        char key=(char)getch();
        if(key==27)exit(0);
    }while(InitCD());
}
ReadNrOfTracks(); */
cprintf("Otwarcie plikow ........\n\r");
OpenPaletteFile();
OpenGraphicFile();/* [PORT] Replace CD Audio with Sound
#ifdef SHAREWARE
SetMaxTrack(5);
#else
SetMaxTrack(15);
#endif*/

Init13h();
BlackPalette();
if(InitText13h()){FreeBuffers13h();Close13h();cprintf("Blad inicjacji pamieci na czcionki, lub brak pliku %sFONT.DAT",drive);return 1;} // [PORT] Main must return int

///////// INTRO ///////////////
SetScreen(0);
SDL_snprintf(ss, sizeof(ss), "%sdata/s000.dat",drive); // [PORT] Replace \ with /; Replace sprintf with SDL_snprintf
if(IsFile(ss))g_polanie->PlayFlic(ss);//odtwarza flica USER // [PORT] Replace play with g_polanie->PlayFlic

SDL_snprintf(ss, sizeof(ss),"%sdata/i001.dat",drive); // [PORT] Replace \ with /; Replace sprintf with SDL_snprintf
if(IsFile(ss))SND.PlayWAV(ss); // [PORT] Replace PlayWav with PlayWAV
SDL_snprintf(ss, sizeof(ss),"%sdata/s001.dat",drive); // [PORT] Replace \ with /; Replace sprintf with SDL_snprintf
if(IsFile(ss))g_polanie->PlayFlic(ss);//odtwarza Drachma // [PORT] Replace play with g_polanie->PlayFlic
SDL_snprintf(ss, sizeof(ss),"%sdata/i001.dat",drive); // [PORT] Replace \ with /; Replace sprintf with SDL_snprintf
if(IsFile(ss))SND.StopWAV(); // [PORT] Replace EnvPlayWav with StopWAV

SDL_snprintf(ss, sizeof(ss),"%sdata/i002.dat",drive); // [PORT] Replace \ with /; Replace sprintf with SDL_snprintf
if(IsFile(ss))SND.PlayWAV(ss); // [PORT] Replace PlayWav with PlayWAV
SDL_snprintf(ss, sizeof(ss),"%sdata/s002.dat",drive); // [PORT] Replace \ with /; Replace sprintf with SDL_snprintf
if(IsFile(ss))g_polanie->PlayFlic(ss);//odtwarza flica Intro // [PORT] Replace play with g_polanie->PlayFlic
SDL_snprintf(ss, sizeof(ss),"%sdata/i002.dat",drive); // [PORT] Replace \ with /; Replace sprintf with SDL_snprintf
if(IsFile(ss))SND.StopWAV(); // [PORT] Replace EnvPlayWav with StopWAV

///////////////////////////////////
SetScreen(1);
SND.PlayTrack(2); // [PORT] Play tracks in now in SND
InitPicture();      //screen=1
show=1;
diff=1;
do
{
  SetScreen(0);
  if(show)
  {
      ClearScreen13h();
      ShowMainMenu();
  }
  show=0;
  MouseEngine();
  MainMenuDispatchEvent();

}while(!endGame && !g_polanie->IsExiting()); // [PORT] Add g_polanie->IsExiting()
DownPalette(7);
//=======Zakonczenie programu===========
FreeBuffers13h();
FreeMemory();
FreeVirtualScreen();
ClosePaletteFile();
CloseGraphicFile();
Close13h();
//DeInitCD(); // [PORT] Replace CD Audio with Sound
}
/////////////////////////////////////////////////////////////////////////
//
//    Obsluga menu
//
////////////////////////////////////////////////////////////////////////
void MainMenuDispatchEvent(void)
{
if(mouse.IsInBoundary(20,130,130,151)||mouse.Key==9579) //k=9579 q=4209 // Replace MWindow with IsInBoundary
    {
    endGame=1;
    }

if(mouse.IsInBoundary(20,45,130,70)||mouse.Key==12654) // new Game // Replace MWindow with IsInBoundary
    {
    NewGame();
    show=1;
    }
if(mouse.IsInBoundary(20,90,130,115)||mouse.Key==4471) //W=4471 Wczytaj gre L=9836 // Replace MWindow with IsInBoundary
    {

        if(!LoadGame())
        {
            Battle(2);
            endGame=0;
            show=1;
            SND.PlayTrack(2); // [PORT] Play tracks in now in SND
        }
        else
        {
            endGame=0;
            show=1;
        }
        return;
    }


}
////////////////////////////////////////////////////////////////////////
//   Przyciski
////////////////////////////////////////////////////////////////////////
void PressButton(int B,int P)
{
switch(B)
      {
       case 1:PutImage13h(108,30,Buttons[P],1);break;    //zachowaj  save1
       case 2:PutImage13h(108,57,Buttons[P],1);break;    //wgraj     save2
       case 3:PutImage13h(108,87,Buttons[P],1);break;    //restart   save3
       case 4:PutImage13h(108,114,Buttons[P],1);break;   //continue  save4
       case 5:PutImage13h(108,142,Buttons[P],1);break;  //koniec    anuluj
       case 11:PutImage13h(274,38,Buttons[P],1);break;   //stop      mag
       case 12:PutImage13h(274,58,Buttons[P],1);break;   //move      lucznik
       case 13:PutImage13h(274,78,Buttons[P],1);break;   //atak      rycerz
       case 14:PutImage13h(274,98,Buttons[P],1);break;   //          krowa
       case 15:PutImage13h(274,118,Buttons[P],1);break;  //          mur
       case 16:PutImage13h(274,138,Buttons[P],1);break;  //          mur
       }
g_polanie->ProcessEvents(); // [PORT] Add here event processing to show the pressing effect
}
////////////////////////////////////////////////////////////////////////
//    Myszka
///////////////////////////////////////////////////////////////////////

void MouseEngine()
{

  g_polanie->ProcessEvents(); //mouse.ReadMouse13h(); // [PORT] Instead of ReadMouse13h, we need to process events
  if(mouse.X>300){mouse.X=300;/*mouse.GMoveCursor(600,mouse.Y);*/} // [PORT] Remove moving cursor
  if(mouse.Y>180){mouse.Y=180;/*mouse.GMoveCursor(mouse.X*2,180);*/} // [PORT] Remove moving cursor


  mouse.oldX=mouse.X;
  mouse.oldY=mouse.Y;
  GetImage13h(mouse.X,mouse.Y,mouse.X+15,mouse.Y+14,Mysz[0]);
  PutImage13h(mouse.X,mouse.Y,buttons[6],1);
  do
  {
      if(mouse.oldX!=mouse.X||mouse.oldY!=mouse.Y)
      {
          PutImage13h(mouse.oldX,mouse.oldY,Mysz[0],0);
          if(mouse.X>300){mouse.X=300;/*mouse.GMoveCursor(600,mouse.Y);*/} // [PORT] Remove moving cursor
          if(mouse.Y>180){mouse.Y=180;/*mouse.GMoveCursor(mouse.X*2,180);*/} // [PORT] Remove moving cursor
          mouse.oldX=mouse.X;
          mouse.oldY=mouse.Y;
          GetImage13h(mouse.X,mouse.Y,mouse.X+15,mouse.Y+14,Mysz[0]);
          PutImage13h(mouse.X,mouse.Y,buttons[6],1);
      }
      //CheckCD(); // [PORT] Replace CD Audio with Sound
      g_polanie->ProcessEvents(); // [PORT] Instead of ReadMouse13h, we need to process events
  }while(!mouse.IsInputReady() && !g_polanie->IsExiting()); // [PORT] Replace GetMsg13h with IsInputReady; Add && !g_polanie->IsExiting()
  PutImage13h(mouse.oldX,mouse.oldY,Mysz[0],0);

}
////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////
void PokazOczy()
{
    char c1=1,c2=2;
    for(int i=0;i<3;i++)
    {
        if(i==diff)
        {
            c1=255;
            c2=254;
        }
        else
        {
            c1=1;
            c2=2;
        }
        PutPixel13h(260,34+i*43,c1);
        PutPixel13h(259,34+i*43,c2);
        PutPixel13h(261,34+i*43,c2);

        PutPixel13h(269,34+i*43,c1);
        PutPixel13h(268,34+i*43,c2);
        PutPixel13h(270,34+i*43,c2);
    }
}
////////////////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////////////////////////////
void NewGame()
{
    int i=0;

    DownPalette(1);
    ShowPicture(10,0);
    ShowPicture(24,100);
    LoadExtendedPalette(10);
    PokazOczy();
    RisePalette(1);
    level=1;
    do{g_polanie->ProcessEvents();}while(mouse.IsInputReady()); // [PORT] Instead of ReadMouse13h, we need to process events; Replace GetMsg13h with IsInputReady
    do
    {
        MouseEngine();
        if(mouse.ClickCount(0)) // [PORT] Replace Ile with ClickCount
        {
            if(mouse.IsInBoundary(44,10,210,32)) // [PORT] Replace MWindow with IsInBoundary
            {
                SND.StopTrack(); // [PORT] Replace StopPlaying with SND.StopTrack
                i=1;
                level=15;
                //play("data\\s002.dat");
                char ss[50];
                SDL_snprintf(ss, sizeof(ss), "%sdata/i003.dat",drive); // [PORT] Replace \\ with /; Replace sprintf with SDL_snprintf
                if(IsFile(ss))SND.PlayWAV(ss); // [PORT] Replace PlayWav with PlayWAV
                SDL_snprintf(ss, sizeof(ss),"%sdata/s003.dat",drive); // [PORT] Replace \\ with /; Replace sprintf with SDL_snprintf
                if(IsFile(ss))g_polanie->PlayFlic(ss);//odtwarza flica Intro Daniel // [PORT] Replace play with g_polanie->PlayFlic
                SND.StopWAV(); // [PORT] Replace EndPlayWav with StopWAV

            }
            if(mouse.IsInBoundary(44,40,210,62)) // [PORT] Replace MWindow with IsInBoundary
            {
                i=2;
                level=26;
              //  play("data\\s003.dat");
            }
            if(mouse.IsInBoundary(44,70,210,92)) // [PORT] Replace MWindow with IsInBoundary
            {
                i=2;
                level=31;
             //   play("data\\s004.dat");
            }
            if(mouse.IsInBoundary(44,100,210,122)) // [PORT] Replace MWindow with IsInBoundary
            {
                i=2;
                level=36;
              //  play("data\\s005.dat");
            }
            if(mouse.IsInBoundary(44,130,210,152)) // [PORT] Replace MWindow with IsInBoundary
            {
                i=2;
                level=42;
             //   play("data\\s006.dat");
            }
            if(mouse.IsInBoundary(44,160,210,182)) // [PORT] Replace MWindow with IsInBoundary
            {
                i=2;
                level=47;
             //   play("data\\s007.dat");
            }
            /////////
            if(mouse.IsInBoundary(235,160,304,182)) // [PORT] Replace MWindow with IsInBoundary
            {
                return;
            }
           // diff
           for(int i=0;i<3;i++)

           if(mouse.IsInBoundary(250,(34+i*43)-10,270,(34+i*43)+20)) // [PORT] Replace MWindow with IsInBoundary
            {
                diff=i;
                PokazOczy();

            }

        }
    }while(!i);

    if(i==1)
    {
        SND.PlayTrack(3); // [PORT] Play tracks in now in SND
        ShowText(1,3); //show start
        for(i=0;i<25;i++)prowintion[i]=prowintionInit[i];
        level=15;
        Battle(1);
        endGame=0;
        SND.PlayTrack(2); // [PORT] Play tracks in now in SND
        return;
    }
    else
    {
        Battle(1);
        endGame=0;
        SND.PlayTrack(2); // [PORT] Play tracks in now in SND
        return;
    }
}
/* [PORT] Replace IsFile with SDL-friendly alternative
int IsFile(char*name)
{
    FILE*f=fopen(name,"rb");
    if(f!=NULL)
    {
        fclose(f);
        return 1;
    }
    return 0;
}*/
int IsFile(char *name)
{
    return SDL_GetPathInfo(g_polanie->GetFilePath(name), nullptr);
}
// END [PORT] Replace IsFile with SDL-friendly alternative
