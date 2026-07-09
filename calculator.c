#include <Keypad.h>
#include <LiquidCrystal.h>
#include <math.h>

// Initialize the LCD (pins RS, EN, D4, D5, D6, D7)
LiquidCrystal lcd(1, 0, 5, 4, 3, 2);

// Define keypad rows and columns
const byte ROWS = 4; // Four rows
const byte COLS = 4; // Four columns

// Define the keymap for the keypad
char keys[ROWS][COLS] = {
  {'1', '2', '3', '+'},
  {'4', '5', '6', '-'},
  {'7', '8', '9', '*'},
  {'C', '0', '=', '/'}
};

// Connect keypad row and column pins
byte rowPins[ROWS] = {13, 12, 11, 10}; // Connect to the row pinouts of the keypad
byte colPins[COLS] = {9, 8, 7, 6}; // Connect to the column pinouts of the keypad

// Create the Keypad object
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

String input = "";
bool resetInput = true;

void setup() {
  lcd.begin(16, 2); // Initialize a 16x2 LCD
  lcd.print("Calculator");
  delay(2000);
  lcd.clear();
}

int precedence(char op) {
  if (op == '+' || op == '-') {
    return 1;
  }
  if (op == '*' || op == '/') {
    return 2;
  }
  return 0;
}

float applyOp(float a, float b, char op) {
  switch (op) {
    case '+': return a + b;
    case '-': return a - b;
    case '*': return a * b;
    case '/': return (b != 0) ? a / b : NAN;
  }
  return 0;
}

float evaluateExpression(String expr) {
  int i = 0;
  float values[100];
  char ops[100];
  int valuesTop = -1, opsTop = -1;

  while (i < expr.length()) {
    if (expr[i] == ' ') {
      i++;
      continue;
    }

    if (expr[i] >= '0' && expr[i] <= '9') {
      // If the character is a digit, extract the number
      String num = "";
      while (i < expr.length() && expr[i] >= '0' && expr[i] <= '9') {
        num += expr[i++];
      }
      values[++valuesTop] = num.toFloat();
    } else if (expr[i] == '+' || expr[i] == '-' || expr[i] == '*' || expr[i] == '/') {
      // If the character is an operator, apply the previous operators if necessary
      while (opsTop >= 0 && precedence(ops[opsTop]) >= precedence(expr[i])) {
        float val2 = values[valuesTop--];
        float val1 = values[valuesTop--];
        char op = ops[opsTop--];
        values[++valuesTop] = applyOp(val1, val2, op);
      }
      ops[++opsTop] = expr[i++];
    }
  }

  // Apply remaining operators
  while (opsTop >= 0) {
    float val2 = values[valuesTop--];
    float val1 = values[valuesTop--];
    char op = ops[opsTop--];
    values[++valuesTop] = applyOp(val1, val2, op);
  }

  return values[valuesTop];
}


void loop() {
  char key = keypad.getKey();

  if (key) {
    if (key >= '0' && key <= '9') {
      input += key;
      lcd.clear();
      lcd.print(input);
    } else if (key == '+' || key == '-' || key == '*' || key == '/') {
      input += key;
      lcd.clear();
      lcd.print(input);
    } else if (key == '=') {
      lcd.clear();
      float result = evaluateExpression(input);
      if (isnan(result)) {
        lcd.print("Err: Div by 0");
      } else {
        lcd.print(input);
        lcd.setCursor(0, 1);
        lcd.print("= ");
        lcd.print(result, 2);  // Limiting to 2 decimal places
      }
      input = "";  // Reset input after calculation
    } else if (key == 'C') {
      lcd.clear();
      input = "";
      lcd.print("Cleared");
      delay(1000);
      lcd.clear();
    }
  }
}