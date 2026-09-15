#ifndef POLANIE_CD_H
#define POLANIE_CD_H

#ifdef __cplusplus
extern "C"
{
#endif

int  InitCD(void);
void setVolume(int); //ustawia od 0 do 5        //!
int  getVolume(void); //zwraca od zero do 5          //!
void CheckCD(void); //sprawdza czy jest grany odpowiedni track i ewentualnie nawraca

int  PlayNext(void);
int  PlayPrevious(void);
int  PlayTrack(int);
void DeInitCD(void);
int  StopPlaying(void);
void SetMaxTrack(int);

void ReadNrOfTracks(void);
int  GetCurrentTrack(void);

void BigOffCDAudio(void);
void BigOnCDAudio(void);

void OffCDAudio(void);
void OnCDAudio(void);

#ifdef __cplusplus
}
#endif

#endif //POLANIE_CD_H
