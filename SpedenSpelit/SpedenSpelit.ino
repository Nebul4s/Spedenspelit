
// Määritellään globaalit muuttujat ym.
volatile int randomNumbers[100];
volatile int userNumbers[100];
volatile int temp[100];

volatile bool button8Falling = true;
volatile bool button9Falling = true;
volatile bool button10Falling = true;
volatile bool button11Falling = true;
volatile bool button12Falling = true;
volatile uint8_t buttonNumber = 0;
void initButtonsAndButtonInterrupts();
bool gameState = false;
int numerolista[10] = { 1, 79, 18, 6, 76, 36, 32, 15, 0, 4 };

const int reset = 6;
const int shiftClock = 5;
const int latchClock = 4;
const int outEnable = 3;
const int serialInput = 2;

bool gameOver = false;
uint8_t result = 0;


volatile int randomCounter = 0;
volatile int playedNumbers = 0;
volatile bool timeToCheckGameStatus = false;
int buttonPressCount;

void setup() {
  /*
    Initialize here all modulesc:\Users\kosti\Downloads\SpedeSpelitBaseline\SpedenSpelit.h
  */
  Serial.begin(9600);



  initButtonsAndButtonInterrupts();
  initializeDisplay();
  initializeLeds();
}

void loop() {

  // Tarkistaa, että jos timeToCheck ... on tosi, suoritetaan checkGame aliohjelma.
  if (timeToCheckGameStatus) {
    checkGame(buttonPressCount);
  }
  // Tarkistaa onko määritelty aloitusnappi painettu
  if (buttonNumber == 5 && !gameState) {
    Serial.println("!!Start game!!");
    startTheGame();
  }
}


ISR(TIMER1_COMPA_vect) {
  // Laskee pelattujen numeroiden määrän, sekä nostaa pelinopeutta 10%:lla joka kymmenes numero
  Serial.println("timer");
  //Serial.print("timer1");
  if (playedNumbers >= 10) {
    playedNumbers = 0;
    OCR1A = OCR1A * 0.9;
  }
  playedNumbers++;
  // Tämä generoi satunnaisia numeroita 0-3 välillä, sekä tallettaa satunnaisen luvun alkioon josta verrata pelaajan syötettä vastaan



  /* int result = random(0, 4);
    int tempIndex = randomCounter - 1
    if(randomNumbers[randomCounter] != temp[tempIndex]{
            randomNumbers[randomCounter] = result;
    }

  */
  randomNumbers[randomCounter] = random(0, 4);
  /*if(randomNumbers[randomCounter] != temp){
        Serial.println(temp);
    }*/

checkArr(randomNumbers);

  clearAllLeds();
  setLed(randomNumbers[randomCounter]);
  randomCounter++;
}

void initializeTimer(void) {
  // Asettaa timer1:n rekisterit oikeille arvoille pelin alkaessa.
  TCCR1A = 0;  // Asetetaan Timer1:n rekisterit nollaksi
  TCCR1B = 0;  // Sama Timer2:lle
  TCNT1 = 0;   // Asetetaan Timer1:n laskuri nollaksi

  OCR1A = 15624;                        // Asetetaan vertailuarvo
  TCCR1B |= (1 << WGM12);               // Timer1 asetetaan CTC tilaan WGM12 Bitin avulla
  TCCR1B |= (1 << CS12) | (1 << CS10);  // Prescaleriksi 1024
  TIMSK1 |= (1 << OCIE1A);              // Sallii Timer1:n vertailu A - keskeytyksen
}
void checkArr(int arr[]){
  for(int i = 0; i<= 100; i++){
    Serial.print(arr[i]);
    Serial.print(" ");
  }
}

void checkGame(int buttonPressCount) {
  // Vertailee randomNumbers sekä userNumbers alkioita toisiinsa, mikäli löytyy ero, peli päätetään.
  Serial.println("!!!!!!!!check game!!!!!!!");
  for (int i = 0; i <= buttonPressCount; i++) {


    if (randomNumbers[i] != userNumbers[i]) {

      Serial.println("Väärää nappia painettu, peli päätetään.");
      gameState = false;
      stopTheGame();
    } else Serial.println("Oikeaa nappia painettu, peli jatkuu.");
    timeToCheckGameStatus = false;

    result = buttonPressCount;
    showResult(result);
  }
}

void initializeGame() {
  // Alustetaan tarvittavat muuttujat ym. joilla aloittaa peli.
  gameState = true;
  Serial.println("!! initialzieGame!!");
  initializeTimer();
  randomSeed(analogRead(A0));  // Käyttää A0 porttia luodakseen satunnaisen "Siemenen", jolla generoida satunnaisia numeroita.

  volatile int randomCounter = 0;
  int playerNumbers = 0;
  bool timeToCheckGameStatus = false;
  buttonPressCount = 0;  // Ykkönen, jotta taulukot olisivat synkronoitu ja molemmat alkavat "ensimmäisestä" alkiosta, eikä 0 alkiosta.
  result = 0;
  gameOver = false;

  //Nämä resetoi jokaisen alkion luvun nollaksi
  for (int i = 0; i < 100; i++) {
    randomNumbers[i] = 0;
    userNumbers[i] = 0;
  }
  showResult(0);
}

void startTheGame() {
  initializeGame();         // Kutsu aliohjelmaa
  TIMSK1 |= (1 << OCIE1A);  // Enabloi Timer1:sen käytön
}

void stopTheGame() {
  clearAllLeds();
  TIMSK1 &= ~(1 << OCIE1A);  // Disabloi Timer1:sen käytön
  gameOver = true;
}



//Buttonsmodule.cpp

void initButtonsAndButtonInterrupts() {
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
  pinMode(11, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);
  PCICR |= B00000001;
  PCMSK0 |= B00011111;
}


ISR(PCINT0_vect) {
  if (digitalRead(8) == LOW && button8Falling == true) {
    Serial.println("nappi 8 alhaalla");
    button8Falling = false;
    buttonNumber = 0;
    if (!gameOver) {  // Ja tämä nimenomaan tarkistaa että onko peli päättynyt vai ei, jotta nappeja ei voi painaa pelin päätyttyä, ihan varalta.
      userNumbers[buttonPressCount] = buttonNumber;
      buttonPressCount++;
      timeToCheckGameStatus = true;
    }
  } else if (digitalRead(8) == HIGH && button8Falling == false) {
    Serial.println("nappi 8 ylhäällä");
    button8Falling = true;
  }
  if (digitalRead(9) == LOW && button9Falling == true) {
    Serial.println("nappi 9 alhaalla");
    button9Falling = false;
    buttonNumber = 1;
    if (!gameOver) {  // Ja tämä nimenomaan tarkistaa että onko peli päättynyt vai ei, jotta nappeja ei voi painaa pelin päätyttyä, ihan varalta.
      userNumbers[buttonPressCount] = buttonNumber;
      buttonPressCount++;
      timeToCheckGameStatus = true;
    }
  } else if (digitalRead(9) == HIGH && button9Falling == false) {
    Serial.println("nappi 9 ylhäällä");
    button9Falling = true;
  }
  if (digitalRead(10) == LOW && button10Falling == true) {
    Serial.println("nappi 10 alhaalla");
    button10Falling = false;
    buttonNumber = 2;
    if (!gameOver) {  // Ja tämä nimenomaan tarkistaa että onko peli päättynyt vai ei, jotta nappeja ei voi painaa pelin päätyttyä, ihan varalta.
      userNumbers[buttonPressCount] = buttonNumber;
      buttonPressCount++;
      timeToCheckGameStatus = true;
    }
  } else if (digitalRead(10) == HIGH && button10Falling == false) {
    Serial.println("nappi 10 ylhäällä");
    button10Falling = true;
  }
  if (digitalRead(11) == LOW && button11Falling == true) {
    Serial.println("nappi 11 alhaalla");
    button11Falling = false;
    buttonNumber = 3;
    if (!gameOver) {  // Ja tämä nimenomaan tarkistaa että onko peli päättynyt vai ei, jotta nappeja ei voi painaa pelin päätyttyä, ihan varalta.
      userNumbers[buttonPressCount] = buttonNumber;
      buttonPressCount++;
      timeToCheckGameStatus = true;
    }
  } else if (digitalRead(11) == HIGH && button11Falling == false) {
    Serial.println("nappi 11 ylhäällä");
    button11Falling = true;
  }
  if (digitalRead(12) == LOW && button12Falling == true) {
    Serial.println("nappi 12 alhaalla");
    button12Falling = false;
    buttonNumber = 5;

  } else if (digitalRead(12) == HIGH && button12Falling == false) {
    Serial.println("nappi 12 ylhäällä");
    button12Falling = true;
  }
}

//Display.cpp

void initializeDisplay(void) {
  pinMode(reset, OUTPUT);
  pinMode(shiftClock, OUTPUT);
  pinMode(latchClock, OUTPUT);
  pinMode(outEnable, OUTPUT);
  pinMode(serialInput, OUTPUT);

  digitalWrite(outEnable, LOW);
  digitalWrite(reset, HIGH);
}

void writeByte(uint8_t number, bool last) {
  digitalWrite(latchClock, LOW);

  uint8_t luku = ~numerolista[number];
  for (int i = 0; i < 8; i++) {
    digitalWrite(shiftClock, LOW);
    int bitti = luku >> i;
    if (bitti & 1 == 1) {
      digitalWrite(serialInput, HIGH);
    } else
      digitalWrite(serialInput, LOW);

    digitalWrite(shiftClock, HIGH);
  }
  if (last) {
    digitalWrite(latchClock, HIGH);
  }
}


void writeHighAndLowNumber(uint8_t tens, uint8_t ones) {
  writeByte(ones, false);
  writeByte(tens, true);
}

void showResult(byte result) {
  if (result < 10) {
    writeByte(result, false);
    writeByte(0, true);
  } else {
    uint8_t ykkoset = result % 10;
    uint8_t kympit = (result - ykkoset) / 10;
    writeHighAndLowNumber(kympit, ykkoset);
  }
}

//Leds.cpp

void initializeLeds() {
  // see requirements for this function from leds.h
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(A5, OUTPUT);
}

void setLed(byte ledNumber) {
  // see requirements for this function from leds.h
  if (ledNumber == 0) {
    digitalWrite(A2, HIGH);
  } else if (ledNumber == 1) {
    digitalWrite(A3, HIGH);
  } else if (ledNumber == 2) {
    digitalWrite(A4, HIGH);
  } else if (ledNumber == 3) {
    digitalWrite(A5, HIGH);
  }
}


void clearAllLeds() {
  // see requirements for this function from leds.h
  digitalWrite(A2, LOW);
  digitalWrite(A3, LOW);
  digitalWrite(A4, LOW);
  digitalWrite(A5, LOW);
}

void setAllLeds() {
  // see requirements for this function from leds.h
  digitalWrite(A2, HIGH);
  digitalWrite(A3, HIGH);
  digitalWrite(A4, HIGH);
  digitalWrite(A5, HIGH);
}


//Leds.h


