//#define SERIAL_ENABLE

int period = 10; // ms
int cont = 0;
int cont2 = 0;
int cont3 = 0;
int state = 0;

const int button_L = 4;
const int button_R = A6;
int button_L_State = 0;
int button_R_State = 0;
int button_L_State_ant = 0;
int button_R_State_ant = 0;

const int led_L_green =  1;
const int led_L_yellow =  0;
const int led_R_green =  A1;
const int led_R_yellow =  A0;

const int display1_A =  8;
const int display1_B =  5;
const int display1_C =  6;
const int display1_D =  7;
const int display2_A =  13;
const int display2_B =  10;
const int display2_C =  11;
const int display2_D =  12;

const int heater_L = 3;
const int heater_R = 9;

const int Tsensor_L = A3;
const int Tsensor_R = A2;
float bit_T_L = 0, bit_T_R = 0;
float T_L = 0, T_R = 0;
int T = 0;
int T_min = 0;
int T_max = 80;
float p_L = 0, p_R = 0;
float last_p_L = 0, last_p_R = 0;
float i_L = 0, i_R = 0;
float limit_int = 180;
float d_L = 0, d_R = 0;
float kp = 1000;
float ki = 0;
float kd = 0;
float controlT_L = 0;
float controlT_R = 0;
float Pmax = 255; // Máximum PWM output for transistors (between 0 & 255)

const int OVERSAMPLENR = 3;

const float temptable[][2] = {
   {1, 713},
   {17, 300},
   {20, 290},
   {23, 280},
   {27, 270},
   {31, 260},
   {37, 250},
   {43, 240},
   {51, 230},
   {61, 220},
   {73, 210},
   {87, 200},
   {106, 190},
   {128, 180},
   {155, 170},
   {189, 160},
   {230, 150},
   {278, 140},
   {336, 130},
   {402, 120},
   {476, 110},
   {554, 100},
   {635, 90},
   {713, 80},
   {784, 70},
   {846, 60},
   {897, 50},
   {937, 40},
   {966, 30},
   {986, 20},
   {1000, 10},
   {1010, 0}
};

void setup() {
  pinMode(button_L, INPUT);      
//  pinMode(button_R, INPUT);  // analog read of button_R

#ifndef SERIAL_ENABLE
  pinMode(led_L_green, OUTPUT);
  pinMode(led_L_yellow, OUTPUT);
  pinMode(led_R_green, OUTPUT);
  pinMode(led_R_yellow, OUTPUT);
#endif

  pinMode(display1_A, OUTPUT);
  pinMode(display1_B, OUTPUT);
  pinMode(display1_C, OUTPUT);
  pinMode(display1_D, OUTPUT);
  pinMode(display2_A, OUTPUT);
  pinMode(display2_B, OUTPUT);
  pinMode(display2_C, OUTPUT);
  pinMode(display2_D, OUTPUT);

#ifdef SERIAL_ENABLE
  Serial.begin(9600); 
#endif
}

void loop() {
/******************************* BUTTONS READING *******************************/
  button_L_State = digitalRead(button_L);
  button_R_State = analogRead(button_R);
  if(button_R_State > 512)
    button_R_State = HIGH;
  else
    button_R_State = LOW;

  if(button_L_State == LOW && button_L_State_ant == HIGH && T > T_min)
  {
    T = T - 5;
    state = 1;
    cont = 0;
  }
  else if(button_R_State == LOW && button_R_State_ant == HIGH && T < T_max)
  {
    T = T + 5;
    state = 1;
    cont = 0;
  }
  
  button_L_State_ant = button_L_State;
  button_R_State_ant = button_R_State;

  if(cont3 >= 500)
  {
/******************************* READING AND AVERAGE TEMPERATURE *******************************/
    bit_T_L = 0;
    bit_T_R = 0;
    for(int i=0; i<OVERSAMPLENR; i++)
    {
      bit_T_L = bit_T_L + analogRead(Tsensor_L);
      bit_T_R = bit_T_R + analogRead(Tsensor_R);
    }
    bit_T_L = bit_T_L/OVERSAMPLENR;
    bit_T_R = bit_T_R/OVERSAMPLENR;

    T_L = bit2deg(bit_T_L);
    T_R = bit2deg(bit_T_R);
  
/******************************* TEMPERATURE PID *******************************/
    p_L = T - T_L;
    p_R = T - T_R;
  
    d_L = p_L - last_p_L;
    d_R = p_R - last_p_R;
  
    i_L = i_L + p_L;
    i_R = i_R + p_R;
    
    if(i_L > limit_int)
      i_L = limit_int;
    if(i_R > limit_int)
      i_R = limit_int;
  
    last_p_L = p_L;
    last_p_R = p_R;
    
    controlT_L = p_L * kp + i_L * ki + d_L * kd;
    controlT_R = p_R * kp + i_R * ki + d_R * kd;
  
    if(controlT_L > Pmax)
      controlT_L = Pmax;
    else if(controlT_L < 0)
      controlT_L = 0;
    if(controlT_R > Pmax)
      controlT_R = Pmax;
    else if(controlT_R < 0)
      controlT_R = 0;
  
    analogWrite(heater_L, controlT_L);           
    analogWrite(heater_R, controlT_R);
  }

/******************************* LEDS AND DISPLAY *******************************/
  if(abs(T - T_L) < 2)
    writeLED(led_L_green, HIGH);
  else
    writeLED(led_L_green, LOW);
  if(abs(T - T_R) < 2)
    writeLED(led_R_green, HIGH);
  else
    writeLED(led_R_green, LOW);

  if(state == 0)
  {
    if(cont < 1000)
    {
      writeLED(led_L_yellow, LOW);
      writeLED(led_R_yellow, LOW);
      display_T(int(T));
    }
    else if(cont < 2000)
    {
      writeLED(led_L_yellow, HIGH);
      writeLED(led_R_yellow, LOW);
      display_T(int(T_L));
    }
    else
    {
      writeLED(led_L_yellow, LOW);
      writeLED(led_R_yellow, HIGH);
      display_T(int(T_R));
      
      if(cont >= 3000)
        cont = 0;
    }
  }
  else if(state == 1)
  {
    writeLED(led_L_yellow, HIGH);
    writeLED(led_R_yellow, HIGH);
    display_T(int(T));
    
    if(cont >= 1000)
    {
      state = 0;
      cont = 0;
    }
  }

/******************************* SERIAL PORT DATA *******************************/
#ifdef SERIAL_ENABLE
  if(cont2 >= 1000) {
    Serial.print("CONSIGNA: ");                       
    Serial.print(T);
    Serial.print(" / IZQ: ");  
    Serial.print(T_L);
    Serial.print(" / DER: ");  
    Serial.println(T_R);
    
    cont2 = 0;
  }
#endif
  
/******************************* COUNTERS UPDATE *******************************/
  cont += period;
  cont2 += period;
  cont3 += period;
  delay(period);                     
}

float bit2deg(float value) {
  float Temp = 0;
  int i = 0;
  
  for(i = 0; temptable[i][0] < value; i++);

  Temp = (temptable[i-1][1] - temptable[i][1]) / (temptable[i-1][0] - temptable[i][0]) * value
    + temptable[i][1] - (temptable[i-1][1] - temptable[i][1]) / (temptable[i-1][0] - temptable[i][0]) * temptable[i][0];

  return Temp;
}

void writeLED(int led, int value)
{
#ifndef SERIAL_ENABLE
  digitalWrite(led, value);
#endif
}

void display_T(int temp) {
  int decenas = 0, unidades = 0;
  int aux = 0;
  
  decenas = temp / 10;
  unidades = temp - decenas * 10;
  
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
