
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
const int numerolista[10] = { 1, 79, 18, 6, 76, 36, 32, 15, 0, 4 };

const int reset = 6;
const int shiftClock = 5;
const int latchClock = 4;
const int outEnable = 3;
const int serialInput = 2;

bool gameOver = true;
uint8_t result = 0;
bool isFirstNum = true;

volatile int randomCounter = 0;
volatile int playedNumbers = 0;
volatile bool timeToCheckGameStatus = false;
volatile int buttonPressCount;

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
  /*
  // Tarkistaa, että jos timeToCheck ... on tosi, suoritetaan checkGame aliohjelma.
  if (timeToCheckGameStatus) {
    checkGame(buttonPressCount);
  }
  // Tarkistaa onko määritelty aloitusnappi painettu
  if (buttonNumber == 5 && gameOver) {
    Serial.println("!!Start game!!");
    startTheGame();
  }*/
}


ISR(TIMER1_COMPA_vect) {
  // Laskee pelattujen numeroiden määrän, sekä nostaa pelinopeutta 10%:lla joka kymmenes numero

  //Serial.print("timer1");
  if (playedNumbers >= 10) {
    playedNumbers = 0;
    OCR1A = OCR1A * 0.9;
  }
  playedNumbers++;
  // Tämä generoi satunnaisia numeroita 0-3 välillä, sekä tallettaa satunnaisen luvun alkioon josta verrata pelaajan syötettä vastaan



  /*
  volatile bool isFinished = false;
  int var = random(1, 5);


  while(!isFirstNum && !isFinished){

    var = random(1, 5);
        Serial.println(var);
      if(var != randomNumbers[randomCounter]){
        Serial.print("jotian");
        
        randomCounter++;
        randomNumbers[randomCounter] = var;
        isFinished = true;
      }
  }
    if(isFirstNum){
      randomNumbers[randomCounter] = var;
      randomCounter++;
      isFirstNum = false;
       Serial.println(var);
  }


  if(randomNumbers[randomCounter] != temp){
        Serial.println(temp);
    }
  */

  int randomNum = (int)random(1,5);
  if(isFirstNum == false){

    if(randomNum == randomNumbers[randomCounter-1]){

      switch(randomNum){
        case 1:
          randomNum = 2;
          break;
        case 2:
          randomNum = 1;
          break;
        case 3:
          randomNum = 4;
          break;
        case 4:
          randomNum = 3;
          break;
      }
      
    }
    randomNumbers[randomCounter] = randomNum;
  }
  else{
    randomNumbers[randomCounter] = randomNum;
  }
  
  //int randomNum = (int)random(1,5);
  isFirstNum = false;
/*  Serial.print("Random ");
  checkArr(randomNumbers);
  Serial.print("Usernumber: ");
  checkArr(userNumbers);*/
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
void checkArr(int arr[]) {
  for (int i = 0; i <= 100; i++) {
    Serial.print(arr[i]);
    Serial.print(" ");
  }
  Serial.println("");
}

void checkGame(int buttonPressCount) {
  // Vertailee randomNumbers sekä userNumbers alkioita toisiinsa, mikäli löytyy ero, peli päätetään.

  for (int i = 0; i < buttonPressCount; i++) {


    if (randomNumbers[i] != userNumbers[i]) {

      gameOver = true;
      stopTheGame();
    }
  }
  //Serial.print("Random ");
  //checkArr(randomNumbers);
  //Serial.print("Usernumber: ");
  //checkArr(userNumbers);
  if(!gameOver){
      result = buttonPressCount;
  }
  showResult(result);
}

void initializeGame() {
  // Alustetaan tarvittavat muuttujat ym. joilla aloittaa peli.
  initializeTimer();
  randomSeed(analogRead(A0));  // Käyttää A0 porttia luodakseen satunnaisen "Siemenen", jolla generoida satunnaisia numeroita.

  randomCounter = 0;
  playedNumbers = 0;
  bool timeToCheckGameStatus = false;
  buttonPressCount = 0;  // Ykkönen, jotta taulukot olisivat synkronoitu ja molemmat alkavat "ensimmäisestä" alkiosta, eikä 0 alkiosta.
  result = 0;
  gameOver = false;
  isFirstNum = true;
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
  TIMSK1 &= ~(1 << OCIE1A);  // Disabloi Timer1:sen   
  gameOver = true;
  Serial.print("R: ");
  checkArr(randomNumbers);
  Serial.print("U: ");
  checkArr(userNumbers);
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
  delayMicroseconds(1000);
  if (digitalRead(8) == LOW && button8Falling == true) {
    Serial.println("nappi 1 alhaalla");
    button8Falling = false;
    buttonNumber = 1;
    if (!gameOver) {  // Ja tämä nimenomaan tarkistaa että onko peli päättynyt vai ei, jotta nappeja ei voi painaa pelin päätyttyä, ihan varalta.
      userNumbers[buttonPressCount] = buttonNumber;
      buttonPressCount++;
      checkGame(buttonPressCount);
    }
  } else if (digitalRead(8) == HIGH && button8Falling == false) {
   Serial.println("nappi 1 ylhäällä");
    button8Falling = true;
  }
  if (digitalRead(9) == LOW && button9Falling == true) {
   Serial.println("nappi 2 alhaalla");
    button9Falling = false;
    buttonNumber = 2;
    if (!gameOver) {  // Ja tämä nimenomaan tarkistaa että onko peli päättynyt vai ei, jotta nappeja ei voi painaa pelin päätyttyä, ihan varalta.
      userNumbers[buttonPressCount] = buttonNumber;
      buttonPressCount++;
      checkGame(buttonPressCount);
    }
  } else if (digitalRead(9) == HIGH && button9Falling == false) {
    Serial.println("nappi 2 ylhäällä");
    button9Falling = true;
  }
  if (digitalRead(10) == LOW && button10Falling == true) {
   Serial.println("nappi 4 alhaalla");
    button10Falling = false;
    buttonNumber = 4;
    if (!gameOver) {  // Ja tämä nimenomaan tarkistaa että onko peli päättynyt vai ei, jotta nappeja ei voi painaa pelin päätyttyä, ihan varalta.
      userNumbers[buttonPressCount] = buttonNumber;
      buttonPressCount++;
      checkGame(buttonPressCount);
    }
  } else if (digitalRead(10) == HIGH && button10Falling == false) {
   Serial.println("nappi 4 ylhäällä");
    button10Falling = true;
  }
  if (digitalRead(11) == LOW && button11Falling == true) {
   Serial.println("nappi 4 alhaalla");
    button11Falling = false;
    buttonNumber = 3;
    if (!gameOver) {  // Ja tämä nimenomaan tarkistaa että onko peli päättynyt vai ei, jotta nappeja ei voi painaa pelin päätyttyä, ihan varalta.
      userNumbers[buttonPressCount] = buttonNumber;
      buttonPressCount++;
      checkGame(buttonPressCount);
     
    }
  } else if (digitalRead(11) == HIGH && button11Falling == false) {
  Serial.println("nappi 3 ylhäällä");
    button11Falling = true;
  }
  if (digitalRead(12) == LOW && button12Falling == true) {
   Serial.println("nappi 5 alhaalla");
    button12Falling = false;
    buttonNumber = 5;
    if (gameOver){ startTheGame();
    }else stopTheGame();


  } else if (digitalRead(12) == HIGH && button12Falling == false) {
  Serial.println("nappi 5 ylhäällä");
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
  writeByte(tens, false);
  writeByte(ones, true);
}

void showResult(byte result) {
  if (result < 10) {
    writeByte(0, false);
    writeByte(result, true);
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
  //Serial.println(ledNumber);
  //Serial.print(randomNumbers[randomCounter]);
  // see requirements for this function from leds.h
  if (ledNumber == 1) {
    digitalWrite(A2, HIGH);
  } else if (ledNumber == 2) {
    digitalWrite(A3, HIGH);
  } else if (ledNumber == 3) {
    digitalWrite(A4, HIGH);
  } else if (ledNumber == 4) {
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
