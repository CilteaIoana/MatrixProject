#include "LedControl.h"  
#include <LiquidCrystal.h>  

const byte rs = 9, en = 8, d4 = 7, d5 = 6, d6 = 5, d7 = 4;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7); // Initilizarea ecranului LCD

const byte buzzerPin = 13; 
const byte LCDbrightnessPin = 3; 
int lcdBrightValue = 136; // Valoarea initiala pentru luminozitatea LCD
const int dinPin = 12, clockPin = 11, loadPin = 10;
const int xPin = A0, yPin = A1; 

LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);

// Variabile pentru controlul luminozitatii si pozitiei
byte matrixBrightness = 2; // Luminozitatea initiala pentru matrice
byte xPos = 1, yPos = 1; // Pozitiile initiale pentru punctul miscator (sarpe)
byte xLastPos = 0, yLastPos = 0; // Ultima pozitie a punctului miscator (sarpe)
const int minThreshold = 200, maxThreshold = 600; // Praguri pentru detectarea miscarii joystick-ului
const byte moveInterval = 500; // Intervalul de timp pentru actualizarea miscarii
unsigned long lastMoved = 0; // Timpul ultimei miscari

const byte matrixSize = 8; 
bool matrixChanged = true; // Variabila pentru a verifica daca matricea s-a schimbat
byte matrix[matrixSize][matrixSize] = {};  // Initializarea matricei cu toate LED-urile stinse

// Enum pentru directiile posibile ale punctului miscator
enum Direction { UP, DOWN, LEFT, RIGHT };
byte direction = RIGHT; // Directia initiala a punctului

// Variabile pentru LED-ul generat random (marul)
byte randomXPos = 0, randomYPos = 0; // Pozitie LED
bool isRandomLEDon = false; // Stare LED
const long blinkInterval = 250; // Intervalul de clipire pentru LED
unsigned long lastBlinkTime = 0; // Ultima clipire

const unsigned long welcomeDuration = 4000; // Durata mesajului de bun venit
unsigned long startTime; // Timpul de inceput pentru afisarea mesajului
bool welcomeDisplayed = false; // Variabila pentru a verifica daca mesajul a fost afisat

const byte menuSize = 4; 
String menuItems[menuSize] = {"Start game", "About", "Mat Brightness", "LCD Brightness"};
byte currentMenuItem = 0; // Elementul curent selectat din meniu
bool gameStarted = false; 
const int buttonPin = 2; 
bool isCountingDown = false; // Variabila pentru a verifica daca se face numaratoarea inversa
unsigned long countdownStartTime; // Timpul de inceput pentru numaratoarea inversa
const unsigned long gameDuration = 15000; // Durata jocului in milisecunde
int score = 0; // Scorul jocului(merel mancate)

const int debounceInterval = 200; // Intervalul pentru debouncing-ul butoanelor
unsigned long lastButtonPress = 0; // Timpul ultimei apasari a butonului

void setup() {
    Serial.begin(9600);  
    lcd.begin(16, 2);    
    lcd.clear();
    lcd.print("Welcome!"); // Afisez mesajul de bun venit pe LCD

    pinMode(buttonPin, INPUT_PULLUP); 
    pinMode(LCDbrightnessPin, OUTPUT); 
    startTime = millis(); 

    lc.shutdown(0, false);  
    lc.setIntensity(0, matrixBrightness); 
    lc.clearDisplay(0);     
}

void loop() {
    analogWrite(LCDbrightnessPin, lcdBrightValue); // Ajustez luminozitate LCD

    if (!welcomeDisplayed && millis() - startTime < welcomeDuration) {
        return; // Continua sa afisezi mesajul de bun venit
    } else if (!welcomeDisplayed) {
        lcd.clear(); 
        welcomeDisplayed = true; // Setez ca mesajul de bun venit a fost afisat
        displayMenu(); // Afisez meniul principal
    }

    // Verifica daca jocul este activ
    if (gameStarted) {
        // Actualizeaza pozitia LED-ului la intervale regulate
        if (millis() - lastMoved > moveInterval) {
            updatePositions(); 
            lastMoved = millis(); 
        }

        // Daca se efectueaza numaratoarea inversa
        if (isCountingDown) {
            runCountdown(); 
            blinkRandomLED(); // Fac LED-ul aleator sa clipeasca

            if (matrixChanged) {
                updateMatrix();
                matrixChanged = false;
            }
        } else {
            gameStarted = false; // Jocul se opreste
            displayMenu(); // Revenire la meniu
        }
    } else {
        navigateMenu();
        // Daca butonul a fost apasat
        if (!digitalRead(buttonPin)) {
            if (millis() - lastButtonPress > debounceInterval) {
                selectMenuItem(); // Select. elementul curent din meniu
                lastButtonPress = millis(); 
            }
        }
    }
}

// Selectarea un element din meniu
void selectMenuItem() {
    delay(200); // Debounce pentru buton pentru a evita citiri multiple
    switch (currentMenuItem) {
        case 0:
            startGame(); // Incepe jocul
            break;
        case 1:
            showAbout(); // Arata informatii despre joc
            break;
        case 2:
            adjustMatrixBrightness(); // Ajusteaza luminozitatea matricei
            break;
        case 3:
            adjustLCDBrightness(); // Ajusteaza luminozitatea LCD
            break;
    }
}

// Functia pentru a afisa informatii despre joc
void showAbout() {
    lcd.clear(); 
    lcd.setCursor(0, 0); // Seteaza cursorul la inceputul ecranului
    lcd.print("Snake Game"); // Afiseaza titlul jocului
    lcd.setCursor(0, 1); // Muta cursorul pe linia a doua
    lcd.print("by CilteaIoana"); // Afiseaza autorul jocului

    // Asteapta pana cand se apasa butonul pentru a reveni la meniu
    bool buttonPressed = false;
    while (!buttonPressed) {
        if (!digitalRead(buttonPin)) {
            buttonPressed = true;
            delay(200);
        }
    }

    lcd.clear(); 
    displayMenu(); // Reafisarea meniului principal
}

// Ajustarea luminozitatii matricei
void adjustMatrixBrightness() {
    bool adjustmentDone = false;
    // Aprindem toate LED-urile inainte de ajustare pentru vizualizare mai buna
    for (int row = 0; row < matrixSize; row++) {
        for (int col = 0; col < matrixSize; col++) {
            lc.setLed(0, row, col, true);
        }
    }

    while (!adjustmentDone) {
        lcd.clear(); 
        lcd.setCursor(0, 0); 
        lcd.print("Matrix - "); 
        lcd.setCursor(9, 0); // Mut cursorul pentru a afisa valoarea
        lcd.print(matrixBrightness); 

        delay(200); 

        int xValue = analogRead(xPin); 

        if (xValue < minThreshold && matrixBrightness > 1) {
            matrixBrightness--;
        } else if (xValue > maxThreshold && matrixBrightness < 15) {
            matrixBrightness++;
        }
        lc.setIntensity(0, matrixBrightness); // Actualizez luminozitatea

        if (!digitalRead(buttonPin)) {
            adjustmentDone = true; // Iesi din modul de ajustare
            delay(200); 
        }
    }

    // Sting toate LED-urile dupa ajustare
    for (int row = 0; row < matrixSize; row++) {
        for (int col = 0; col < matrixSize; col++) {
            lc.setLed(0, row, col, false);
        }
    }

    lcd.clear(); 
    displayMenu(); // Reafiseaza meniul
}

// Ajustarea luminozitatii LCD
void adjustLCDBrightness() {
    bool adjustmentDone = false;
    while (!adjustmentDone) {
        lcd.clear(); 
        lcd.setCursor(0, 0); 
        lcd.print("LCD - "); 
        lcd.setCursor(6, 0); // Mut cursorul pentru a afisa valoarea
        lcd.print(lcdBrightValue); 

        delay(200); 

        int xValue = analogRead(xPin);

        // Ajustez luminozitatea in functie de intrarea joystick-ului
        if (xValue < minThreshold && lcdBrightValue > 17) {
            lcdBrightValue -= 17;
        } else if (xValue > maxThreshold && lcdBrightValue <= (255 - 17)) {
            lcdBrightValue += 17;
        }
        analogWrite(LCDbrightnessPin, lcdBrightValue); // Actualizeaza luminozitatea LCD

        if (!digitalRead(buttonPin)) {
            adjustmentDone = true; // Iesi din modul de ajustare
            delay(200); 
        }
    }

    lcd.clear(); 
    displayMenu(); // Reafisez meniul
}

//Functie facuta cu ajutor (Chat-GPT)
// Afisarea meniului principal
void displayMenu() {
    lcd.clear(); 
    for (byte i = 0; i < 2; i++) { // Parcurge primele doua elemente ale meniului
        byte menuItemIndex = currentMenuItem + i; // Indexul elementului curent
        if (menuItemIndex < menuSize) { 
            lcd.setCursor(0, i); // Setez cursorul pe ecran
            if (menuItemIndex == currentMenuItem) {
                lcd.print(">"); // Afiseaza indicatorul pentru elementul selectat
            } else {
                lcd.print(" "); // Lasa un spatiu pentru elementele neselectate
            }
            lcd.setCursor(1, i); 
            lcd.print(menuItems[menuItemIndex]); // Afiseaza nume functionalitate din meniu
        }
    }
}

// Navigarea in meniu 
void navigateMenu() {
    int xValue = analogRead(xPin); 
    if (xValue < minThreshold) { 
        if (currentMenuItem > 0) { 
            currentMenuItem--; // Mergi la elementul anterior
        }
        displayMenu(); // Reafiseaza meniul
        delay(200); 
    } else if (xValue > maxThreshold) { 
        if (currentMenuItem < menuSize - 1) {
            currentMenuItem++; // Mergi la elementul urmator
        }
        displayMenu(); // Reafiseaza meniul
        delay(200);
    }
}

void startGame() {
    gameStarted = true; // Jocul a inceput
    isCountingDown = true; // Incepe numaratoarea inversa
    countdownStartTime = millis(); // Timpul de start pentru numaratoare
    score = 0; // Resetarea scorului

    // Resetez coordonatele punctului miscator (sarpelui)
    xPos = 1;
    yPos = 1;
    xLastPos = 0;
    yLastPos = 0;

    resetMatrix(); // Resetarea matricei 
    isRandomLEDon = false; 
    randomizeLED(); // Genereaza o noua pozitie aleatorie pentru LED

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Time: 15"); // Timpul ramas pentru joc
    updateScore(); // Afisez scorul initial
}

void resetMatrix() {
    for (int i = 0; i < matrixSize; i++) {
        for (int j = 0; j < matrixSize; j++) {
            matrix[i][j] = 0; // Sting fiecare LED din matrice
        }
    }
    direction = UP; // Directia initiala 
    lc.clearDisplay(0);
}

// Rularea numaratoarei inverse in timpul jocului
void runCountdown() {
    if (!isCountingDown) return; // Daca nu se numara invers, iesi

    unsigned long currentTime = millis();
    if (currentTime - countdownStartTime >= gameDuration) {
        // Daca timpul alocat jocului s-a terminat
        isCountingDown = false; // numaratoarea inversa se opreste
        gameStarted = false; // jocul se opreste
        displayMenu(); // revebim la meniu
    } else {
        // Afisez timpul ramas pe ecran
        lcd.setCursor(0, 0);
        lcd.print("Time: ");
        lcd.print((gameDuration - (currentTime - countdownStartTime)) / 1000);
        lcd.print(" sec");
    }
}

// Clipacirea marului
void blinkRandomLED() {
    if (isRandomLEDon && (millis() - lastBlinkTime > blinkInterval)) {
        matrix[randomXPos][randomYPos] ^= 1; // Schimb starea LED-ului 
        lastBlinkTime = millis(); // Resetez timpul pentru clipirea urmatoare
        matrixChanged = true; //matricea a fost modificata
    }
}

// Aprinderea unul LED random (marului)
void randomizeLED() {
    randomXPos = random(matrixSize);
    randomYPos = random(matrixSize); 
    matrix[randomXPos][randomYPos] = 1; // Aprind LED-ul la noua pozitie random aleasa
    isRandomLEDon = true; // LED-ul aleator este acum aprins
}

// Actualizez starea matricei
void updateMatrix() {
    for (int row = 0; row < matrixSize; row++) {
        for (int col = 0; col < matrixSize; col++) {
            lc.setLed(0, row, col, matrix[row][col]); 
        }
    }

    if (!isRandomLEDon) {
        // Daca nu exista un LED aleator aprins
        randomizeLED();  // Aprindem unul :)
        matrixChanged = true; // Matricea a fost modificata
    }
}

// Functia pentru a actualiza scorul pe ecranul LCD
void updateScore() {
    lcd.setCursor(0, 1); // Seteaza cursorul pe a doua linie a LCD-ului
    lcd.print("Score: ");
    lcd.print(score); // Afiseaza scorul actual
}

// Actualizarea pozitiei punctului miscator pe matrice
void updatePositions() {
    int xValue = analogRead(xPin); 
    int yValue = analogRead(yPin); 

    xLastPos = xPos;
    yLastPos = yPos;

    // Salvez directia in functie de miscarea joystick-ului
    if (xValue < minThreshold && direction != RIGHT) {
        direction = RIGHT; 
    } else if (xValue > maxThreshold && direction != LEFT) {
        direction = LEFT; 
    }

    if (yValue > maxThreshold && direction != DOWN) {
        direction = DOWN; 
    } else if (yValue < minThreshold && direction != UP) {
        direction = UP; 
    }

    // Actualizez pozitia in functie de directia curenta
    switch (direction) {
        case UP:
            yPos = (yPos + 1) % matrixSize;
            break;
        case DOWN:
            yPos = (yPos > 0) ? yPos - 1 : matrixSize - 1;
            break;
        case LEFT:
            xPos = (xPos + 1) % matrixSize;
            break;
        case RIGHT:
            xPos = (xPos > 0) ? xPos - 1 : matrixSize - 1;
            break;
    }

    // Verific daca punctul miscator a ajuns pe pozitia LED-ului random generat (a marului)
    if (xPos == randomXPos && yPos == randomYPos) {
        matrix[randomXPos][randomYPos] = 0; // Sting LED-ul random
        isRandomLEDon = false; // Setez starea LED-ului random ca fiind stins
        tone(buzzerPin, 1000, 100); 
        score++; 
        updateScore();
    }

    // Verific daca pozitia s-a schimbat si actualizez matricea corespunzator
    if (xPos != xLastPos || yPos != yLastPos) {
        matrix[xLastPos][yLastPos] = 0; // Stinge LED-ul de la ultima pozitie
        matrix[xPos][yPos] = 1; // Aprinde LED-ul la noua pozitie
        matrixChanged = true;
    }
}
