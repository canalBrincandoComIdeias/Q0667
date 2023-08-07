// INCLUSAO DA BIBLIOTECA
#include <TimerOne.h>
// ======================

#include <PushButton.h>
#include <LiquidCrystal_I2C.h>

#define potPin1 A0  //Potenciometro da bobinadeira
#define potPin2 A2  // Sensor (Modelo Hall 49e)
#define endereco 0x27  //SDA A4 e SCL A5
#define colunas 16
#define linhas 2
#define tensao1 5     // setado em high para fornecer 5 volts para os módulos
#define tensao2 4     // setado em high para fornecer 5 volts para os módulos
#define tensao3 3     // setado em high para fornecer 5 volts para os módulos
#define pinBotao1 13  //Botões para alterar as variáveis de controle
#define pinBotao2 12  //Botões para alterar as variáveis de controle
#define pinBotao3 11  //Botões para alterar as variáveis de controle
#define pinBotao4 10  //Botões para alterar as variáveis de controle

PushButton botao1(pinBotao1);
PushButton botao2(pinBotao2);
PushButton botao3(pinBotao3);
PushButton botao4(pinBotao4);

LiquidCrystal_I2C lcd(endereco, colunas, linhas);
const int stepPin = 7; // aceleração do Rolo
const int dirPin = 6;  // direção do Rolo

// variavel que vai receber o valor lido do potenciometro e sensor hall
int valPot1 = A1;//potenciometro da bobinadeira
int sensorhall = A2; // sensor hall
long hall;        // Variável que armazena o valor de aceleração ou freio
int ajst;         //valor utilizado para referência do diâmetro do filamento capturado do sensor ao ligar a máquina
int divisor = 10; // divisor para suavisar o freio ou aceleração do motor
static unsigned long delay1;  // variaveis que tem a função de armazenar tempo, para evitar uso da função delay
static unsigned long delay2;;  // variaveis que tem a função de armazenar tempo, para evitar uso da função delay
static unsigned long delay3;
//  static unsigned long delay4;// variaveis que tem a função de armazenar tempo, para evitar uso da função delay
int halcd;

void setup() {
  pinMode (tensao1, OUTPUT);
  pinMode (tensao2, OUTPUT);
  pinMode (tensao3, OUTPUT);

  digitalWrite (tensao1, HIGH);// fornecer tensão para os módulos
  digitalWrite (tensao2, HIGH);// fornecer tensão para os módulos
  digitalWrite (tensao3, HIGH);// fornecer tensão para os módulos

  digitalWrite(dirPin, LOW); // direção
  Serial.begin (9600);
  pinMode(stepPin, OUTPUT); // motor da bobinadeira
  pinMode(dirPin, OUTPUT); // direção da bobinadeira

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("Div Ref Vel Hal");
  lcd.setCursor(0, 1);
  ajst = analogRead(potPin2);  // Ler o Sensor Hall e gravar o o valor para servir de parâmetro para ajuste do filamento, coloque um pedaço de filamento da medida que será repetida durante todo o processo de fabricação.

  // CONFIGURAÇÃO DO TIMER
  Timer1.initialize(); //INICIALIZA O TIMER, PERIODO PADRAO 1000 us
  Timer1.pwm(10, 512, 1000); // CONFIGURA O PWM NA PORTA 10 COM 50%
  // ======================
}
void loop() {
  static unsigned int mostraMicros = 0;
  botao1.button_loop();
  botao2.button_loop();
  botao3.button_loop();
  botao4.button_loop();

  if (botao1.pressed()) {
    ajst = ajst + 1;
  }

  if (botao2.pressed()) {
    ajst = ajst - 1;
  }

  if (botao3.pressed()) {
    divisor = divisor + 1;
  }
  if (botao4.pressed()) {
    divisor = divisor - 1;
  }

  float motorPasso = 0;

  digitalWrite(stepPin, HIGH);
  delayMicroseconds(motorPasso);

  valPot1 = analogRead(potPin1);  //Ler o Potenciometro do rolo
  sensorhall = analogRead(potPin2);  // Ler o Sensor Hall

  digitalWrite(stepPin, LOW);
  delayMicroseconds(motorPasso);


  motorPasso = valPot1 + (hall / divisor);     // Sensor Hall diminui a velocidade caso o filamento afine, ou aumenta caso o filamento engrosse. Somando ou diminuindo ao valor lido pelo potenciometro.//
  // motor(motorPasso*10);

  if ((millis() - delay1) > 5000) {
    if (sensorhall >= (ajst + 1)) { // o loop irá comparar o valor armazenado no setup se o valor no sensor hall for igual ou maior do que o valor armazenado ele acelera.
      hall = hall - 1;
    }

    if (sensorhall <= (ajst - 1)) { // o loop irá comparar o valor armazenado no setup se o valor no sensor hall for igual ou menor do que o valor armazenado ele freia.
      hall =  hall + 1;
    }
    delay1 = millis();
  }

  if ((millis() - delay2) > 5000) {
    Serial.println(motorPasso);
    lcd.print(divisor);  // mostrará o valor aumentando ou diminuindo conforme apertado os botões
    if (divisor < 10) {
      lcd.setCursor(1, 1);
      lcd.print(" ");
    }
    lcd.setCursor(4, 1);
    lcd.print(ajst);  // mostrará o valor aumentando ou diminuindo conforme apertado os botões
    lcd.setCursor(8, 1);
    lcd.print(valPot1);  // Mostrará o valor que acelera ou freia o motor
    lcd.setCursor(13, 1);
    lcd.print(sensorhall); // Mostra o valor lido no sensor hall
    lcd.setCursor(0, 1);
    delay2 = millis();
  }

  // CONTROLE DO MOTOR SEGUNDARIO
  Timer1.setPeriod(max(motorPasso*10, 800)); // MINIMO = 800uS APROX.
}

void motor(long tempoPasso2) { // lógica para movimentar o motor vai e vem
  static unsigned long delay4;
  static bool estadoPulso = LOW;
  if ((micros() - delay4) > tempoPasso2) {
    digitalWrite(stepPin, estadoPulso);
    estadoPulso = !estadoPulso;
    delay4 = micros();
  }
}
