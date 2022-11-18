
/*
Spider Run
*/

#include <LiquidCrystal.h>

// Defines the pins that will be used for the display
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

//bitmap array for the man character
byte man [8]
{ B01110,
  B01010,
  B00100,
  B01110,
  B10101,
  B00100,
  B01010,
  B01010,
};

//character for the stick
byte stick [8]
{
  B00100,
  B00100,
  B00100,
  B00100,
  B00100,
  B00100,
  B00100,
  B11111
};

//character for the hoop
byte hoop [8]
{
  B01110,
  B10001,
  B10001,
  B10001,
  B10001,
  B10001,
  B10001,
  B01110,
};

//character for case when man is in hoop
byte hoopwithman [8]
{
  B01110,
  B11011,
  B10101,
  B11111,
  B10101,
  B10101,
  B11011,
  B01110,
};

const int BUTTON_ENTER = 8;
const int BUTTON_SELECT = 9;

const int MENU_SIZE = 2;
const int LCD_COLUMN = 16;


const int HOOP_CHAR = 0;
const int STICK_CHAR = 1;
const int MAN_CHAR = 2;
const int HOOP_WITH_MAN_CHAR = 3;

const int COLUMN_INDEX_TO_START_MOVE_HOOPS = LCD_COLUMN - 4;
const String ALPHABET[26] = { "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z" };

boolean isPlaying = false;
boolean isShowScore = false;
boolean isManOnGround = true;

int currentIndexMenu = 0;
int score = 0;
int scoreListSize = 0;
String scoreList[20];

int buttonPressedTimes = 0;

void setup() {
  lcd.begin(16, 2);
  lcd.createChar(MAN_CHAR, man);
  lcd.createChar(STICK_CHAR, stick);
  lcd.createChar(HOOP_CHAR, hoop);
  lcd.createChar(HOOP_WITH_MAN_CHAR, hoopwithman);

  pinMode(BUTTON_ENTER, INPUT_PULLUP);
  pinMode(BUTTON_SELECT, INPUT_PULLUP);
}

void loop() {
  lcd.clear();
  handleMenu();

  delay(300);
}

void handleMenu() {
  
  lcd.setCursor(1, 0);
  lcd.write(MAN_CHAR);
  lcd.print(" SPIDEY RUN ");
  lcd.write(MAN_CHAR);
  lcd.setCursor(2, 1);
  lcd.print("START");
  lcd.setCursor(10, 1);
  lcd.print("SCORE");

  if(currentIndexMenu == 0) {
    lcd.setCursor(0, 1);
    lcd.print("->");
  }
  if(currentIndexMenu == 1) {
    lcd.setCursor(8, 1);
    lcd.print("->");
  }

  if (digitalRead(BUTTON_SELECT) == LOW) {
    currentIndexMenu = currentIndexMenu == 0 ? 1 : 0;
  }

  if (digitalRead(BUTTON_ENTER) == LOW) {
    currentIndexMenu == 0 ? startGame() : showScore();
  }
}

void showScore () {
  isShowScore = true;
  delay(200);

  int currentIndex = 0;
  const int lastIndex = scoreListSize - 1;

  printScore(currentIndex, lastIndex);

  while (isShowScore) {
    if (digitalRead(BUTTON_SELECT) == LOW) {
      currentIndex = currentIndex < lastIndex ? currentIndex + 1 : 0;
      printScore(currentIndex, lastIndex);
    }

    if (digitalRead(BUTTON_ENTER) == LOW) {
      isShowScore = false;
    }

    delay(200);
  }
}

void printScore(int index, int lastIndex) {
  lcd.clear();

  if (lastIndex == -1) {
    lcd.print("NO SCORE");
  }
  else {
    lcd.print(scoreList[index]);

    if (index < lastIndex) {
      lcd.setCursor(0, 1);
      lcd.print(scoreList[index + 1]);
    }
  }
}

void startGame () {
  isPlaying = true;
  buttonPressedTimes = 0;
  while (isPlaying) {
    handleGame();
  }
}

void handleGame() {
  lcd.clear();

  // Generate two random distances for the space between the hoops
  int secondPosition = random(4, 9);
  int thirdPosition = random(4, 9);
  int firstHoopPosition = COLUMN_INDEX_TO_START_MOVE_HOOPS;
  
  const int columnIndexToStopMoveHoops = -(secondPosition + thirdPosition);

  // this loop is to make the hoops move, this loop waiting until
  // all the hoops moved
  for (; firstHoopPosition >= columnIndexToStopMoveHoops; firstHoopPosition--) {

    lcd.setCursor(14, 0);
    lcd.print(score);
    
    defineManPosition();

    int secondHoopPosition = firstHoopPosition + secondPosition;
    int thirdHoopPosition = secondHoopPosition + thirdPosition;

    showHoop(firstHoopPosition);
    if(secondHoopPosition<=COLUMN_INDEX_TO_START_MOVE_HOOPS){
      showHoop(secondHoopPosition);
    }
     if(thirdHoopPosition<=COLUMN_INDEX_TO_START_MOVE_HOOPS){
      showHoop(thirdHoopPosition);
    }

    if (firstHoopPosition == 0 || secondHoopPosition == 0 || thirdHoopPosition == 0){
        defineManPosition ();
    }
    
    if (isManOnGround) {
      if (firstHoopPosition == 1 || secondHoopPosition == 1 || thirdHoopPosition == 1) {
        handleGameOver();
        //delay(5000);
        break;
      }
      buttonPressedTimes = 0;

    } else {

      if (firstHoopPosition == 1 || secondHoopPosition ==1 || thirdHoopPosition == 1) {
        showHoopWithMan(1);
      }
      
      buttonPressedTimes++;
    }

    score++;
    delay(int(400*pow(1.009, -1*score)+100));
    handlePauseEvent();
  }
}

void handleGameOver () {
  lcd.clear();
  lcd.print("GAME OVER");

  lcd.setCursor(0, 1);
  lcd.print("SCORE: ");
  lcd.print(score);

  delay(2000);
  saveScore();
}

void handlePauseEvent() {

  if (digitalRead(BUTTON_SELECT) == LOW) {
      while(1) {
        delay(1000);
        if (digitalRead(BUTTON_SELECT) == LOW) {
          break;
        }
      }
    }
}

void saveScore () {
  lcd.clear();

  String player_name = "";
  int nameSize = 0;
  int alphabetCurrentIndex = 0;

  lcd.print("TYPE YOUR NAME");

  while (nameSize != 3) {
    lcd.setCursor(nameSize, 1);
    lcd.print(ALPHABET[alphabetCurrentIndex]);

    if (digitalRead(BUTTON_SELECT) == LOW) {
      alphabetCurrentIndex = alphabetCurrentIndex != 25 ? alphabetCurrentIndex + 1 : 0;
    }

    if (digitalRead(BUTTON_ENTER) == LOW) {
      player_name += ALPHABET[alphabetCurrentIndex];

      nameSize++;
      alphabetCurrentIndex = 0;
    }

    delay(300);
  }

  scoreList[scoreListSize] =  player_name + " " + score;
  scoreListSize++;

  isPlaying = false;
  score = 0;
}

void showHoop (int position) {

  lcd.setCursor(position, 1);
  lcd.write(STICK_CHAR);

  // clean the previous position
  lcd.setCursor(position + 1, 1);
  lcd.print(" ");
  
  lcd.setCursor(position, 0);
  lcd.write(HOOP_CHAR);

  // clean the previous position
  lcd.setCursor(position + 1, 0);
  lcd.print(" ");
}

void showHoopWithMan (int position) {

  lcd.setCursor(position, 1);
  lcd.write(STICK_CHAR);

  // clean the previous position
  lcd.setCursor(position + 1, 1);
  lcd.print(" ");
  
  lcd.setCursor(position, 0);
  lcd.write(HOOP_WITH_MAN_CHAR);

  // clean the previous position
  lcd.setCursor(position + 1, 0);
  lcd.print(" ");
}

void defineManPosition () {
  int buttonState = digitalRead(BUTTON_ENTER);
  if (buttonPressedTimes!=0 && buttonPressedTimes%3==0) buttonState = 1;
  buttonState == HIGH ? putManOnGround() : putManOnAir();
}

void putManOnGround () {
  lcd.setCursor(1, 1);
  lcd.write(MAN_CHAR);
  lcd.setCursor(1, 0);
  lcd.print(" ");

  isManOnGround = true;
}

void putManOnAir () {
  lcd.setCursor(1, 0);
  lcd.write(MAN_CHAR);
  lcd.setCursor(1, 1);
  lcd.print(" ");

  isManOnGround = false;
}
