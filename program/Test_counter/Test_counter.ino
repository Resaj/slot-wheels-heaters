int period = 1000; // ms
int cont = 0;

const int display1_A =  8;
const int display1_B =  5;
const int display1_C =  6;
const int display1_D =  7;
const int display2_A =  13;
const int display2_B =  10;
const int display2_C =  11;
const int display2_D =  12;

void setup() {
  pinMode(display1_A, OUTPUT);
  pinMode(display1_B, OUTPUT);
  pinMode(display1_C, OUTPUT);
  pinMode(display1_D, OUTPUT);
  pinMode(display2_A, OUTPUT);
  pinMode(display2_B, OUTPUT);
  pinMode(display2_C, OUTPUT);
  pinMode(display2_D, OUTPUT);

  Serial.begin(9600); 
}

void loop() {
  if(cont >= 100)
    cont = 0;
  
  Serial.print(cont);   
  
  display_T(int(cont));

  cont += 1;
  delay(period);
}

void display_T(int temp) {
  int decenas = 0, unidades = 0;
  int aux = 0;
  
  Serial.print("    ");   
  decenas = temp / 10;
  Serial.print(decenas);   
  Serial.print("    ");   
  unidades = temp - decenas * 10;
  Serial.println(unidades);   
  
  if(decenas - 8 >= 0)
  {
    digitalWrite(display1_D, HIGH);
    aux = decenas - 8;
  }
  else
  {
    digitalWrite(display1_D, LOW);
    aux = decenas;
  }
  
  if(aux - 4 >= 0)
  {
    digitalWrite(display1_C, HIGH);
    aux = aux - 4;
  }
  else
    digitalWrite(display1_C, LOW);
    
  if(aux - 2 >= 0)
  {
    digitalWrite(display1_B, HIGH);
    aux = aux - 2;
  }
  else
    digitalWrite(display1_B, LOW);

  if(aux - 1 >= 0)
  {
    digitalWrite(display1_A, HIGH);
    aux = aux - 1;
  }
  else
    digitalWrite(display1_A, LOW);

  if(unidades - 8 >= 0)
  {
    digitalWrite(display2_D, HIGH);
    aux = unidades - 8;
  }
  else
  {
    digitalWrite(display2_D, LOW);
    aux = unidades;
  }
  
  if(aux - 4 >= 0)
  {
    digitalWrite(display2_C, HIGH);
    aux = aux - 4;
  }
  else
    digitalWrite(display2_C, LOW);
    
  if(aux - 2 >= 0)
  {
    digitalWrite(display2_B, HIGH);
    aux = aux - 2;
  }
  else
    digitalWrite(display2_B, LOW);

  if(aux - 1 >= 0)
  {
    digitalWrite(display2_A, HIGH);
    aux = aux - 1;
  }
  else
    digitalWrite(display2_A, LOW);
}
