#include <MIDIUSB.h>
#include "PitchToNote.h"
// definiuje liczbe przyciskow w kontrolerze
#define NUM_BUTTONS  12

//
const uint16_t button1 = 2;
const uint16_t button2 = 3;
const uint16_t button3 = 4;
const uint16_t button4 = 5;
const uint16_t button5 = 6;
const uint16_t button6 = 7;
const uint16_t button7 = 8;
const uint16_t button8 = 9;
const uint16_t button9 = 10;
const uint16_t button10 = 16;
const uint16_t button11 = 14;
const uint16_t button12 = 15;
// zmienne dla potencjometrow. potrzebujemy dwoch zmiennych dla kazdego potencjometru aby sprawdzac czy zaszla zmiana wartosci
int val = 0;
int lastVal = 0;
int val2 = 0;
int lastVal2 = 0;
int val3 = 0;
int lastVal3 = 0;
int val4 = 0;
int lastVal4 = 0;

//kanal midi na ktory bedziemy wysylac
byte midiCh = 0;
// tablica z przyciskami
const uint16_t buttons[NUM_BUTTONS] = {button1, button2, button3, button4, button5, button6, button7, button8, button9, button10, button11, button12};
// tablica wysylanych dzwiekow (korzysta z biblioteki PitchToNote.h)
const byte notePitches[NUM_BUTTONS] = {pitchC2, pitchD2b, pitchD2, pitchE2b, pitchE2, pitchF2, pitchG2b, pitchG2, pitchA2b, pitchA2, pitchB2b, pitchB2};


// 16 bitowa zmienna - bedziemy zmieniać tu pojedyncze bity aby określić czy przycisk jest wciśnięty
uint16_t pressedButtons = 0x00;
// 16 bitowa zmienna gdzie bedziemy przechowywac poprzedni stan kazdego przycisku ( tak jak w zmiennej wyzej wykorzystamy tylko 12 bitow poniewaz mamy tylko 12 przyciskow)
uint16_t previousButtons = 0x00;


void setup() {
  // Ustawienie pinow na tryb input
  for (int i = 0; i < NUM_BUTTONS; i++)
  pinMode(buttons[i], INPUT);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
}


void loop() {
  //wywolanie funkcji
  readButtons();
  playNotes();
  getPots();
}


void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}

void readButtons()
{
  // jesli przycisk jest wcisniety, zmieniam bit odpowiadajacy temu przyciskowi na 1, w przeciwnym razie na 0
  for (int i = 0; i < NUM_BUTTONS; i++)
  {
    if (digitalRead(buttons[i]) == HIGH)
    {
      bitWrite(pressedButtons, i, 1);
      delay(2);
    }
    else
      bitWrite(pressedButtons, i, 0);
  }
}

void playNotes()
{
  // dla kazdego wcisnietego przycisku wysylam odpowiedni sygnal midi
  for (int i = 0; i < NUM_BUTTONS; i++)
  {
    if (bitRead(pressedButtons, i) != bitRead(previousButtons, i))
    {
      if (bitRead(pressedButtons, i))
      {
        bitWrite(previousButtons, i , 1);
        noteOn(0, notePitches[i], 100);
        MidiUSB.flush();
      }
      else
      {
        bitWrite(previousButtons, i , 0);
        noteOff(0, notePitches[i], 0);
        MidiUSB.flush();
      }
    }
  }
}
void getPots()
{
  val = analogRead(A0);  // jesli nastapila zmiana wartosci na potencjometrze funkcja ponizej zostanie wywolana.
   if (val != lastVal)
   {
    //wyslanie na wczesniej ustalony kanal midi wiadomosci control change o numerach 7-10, o wartosci w zakresie 0-127
          controlChange(midiCh, 7, map(val, 0, 1023, 0, 127));
   }
   // aktualizacja wartosci
   lastVal = val;

   val2 = analogRead(A1);
   if (val2 != lastVal2)
   {
    controlChange(midiCh, 8, map(val2, 0, 1023, 0, 127));
   }
   lastVal2 = val2;

   val3 = analogRead(A2);
   if (val3 != lastVal3)
   {
    controlChange(midiCh, 9, map(val3, 0, 1023, 0, 127));
   }
   lastVal3 = val3;

    val4 = analogRead(A3);
   if (val4 != lastVal4)
   {
    controlChange(midiCh, 10, map(val4, 0, 1023, 0, 127));
   }
   lastVal4 = val4;

delay(10);//Dodaje drobny delay aby zminimalizować szanse na zdarzenia typu zmiana wartości kiedy potencjometr zostanie przypadkowo lekko dotkniety
}

//funkcje biblioteki MIDIUSB

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

