// INCLUSAO DA BIBLIOTECA
#include <TimerOne.h>
// ======================

#include <PushButton.h>
#include <LiquidCrystal_I2C.h>

#define endereco 0x27  //SDA A4 e SCL A5
#define colunas 16
#define linhas 2

LiquidCrystal_I2C lcd(endereco, colunas, linhas);

#define potAjst   A0 // Potenciometro de ajst
#define potBob    A1 // Potenciometro da velocidade do bobinadeira
#define potPin    A2 // Potenciometro de velocidade da vai e vem
#define potFreio  A3 // Potenciometro de freio da bobinadeirapotBob    A3 // Potenciometro da velocidade do vai e vem
#define HallDig1    6 // sensor hall digital
#define HallDig2    7  // sensor hall digital
#define bobPin    8 // Aceleração da bobinadeira
#define bobDir    9 // direção da bobinadeira
#define vaiPin    10 // aceleração do vai e vem
#define vaiDir    11 // direção do vai e vem
#define pinBotao1 13  //Botões para alterar as variáveis de controle
#define pinBotao2 12  //Botões para alterar as variáveis de controle
#define pinBotao3 12  //Botões para alterar as variáveis de controle
#define pinBotao4 13  //Botões para alterar as variáveis de controle

PushButton botao1(pinBotao3);
PushButton botao2(pinBotao4);

unsigned long delay1;
unsigned long delay2;
unsigned long delay3;


int freio;  // variável que vai armazenar o valor que vai ser mapeado para ir freiando o motor da bobinadeira ou acelerando conforme o caso
int contUp = 450; // Parte de cima do Freio
int contDw = 430; // Parte de Baixo do Freio
int ajst;   // variavel de teto ou piso para o potenciometro do freio
int freiar;

int valBob;
int tempo = 270;

void setup() {
  Serial.begin(9600);
  pinMode( HallDig1,   INPUT_PULLUP ); // Botão que altera direção
  pinMode( HallDig2,   INPUT_PULLUP ); // Botão que altera direção
  pinMode( bobDir, OUTPUT );// direção do motor pelo drive
  pinMode( bobPin, OUTPUT ); // Controle do motor pelo drive
  pinMode( vaiDir, OUTPUT); // direçao do vai e vem
  pinMode( vaiPin, OUTPUT); // motor do vai e vem
  digitalWrite (bobDir, LOW);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("Tim Lig Ajst Hall");
  lcd.setCursor(0, 1);

  // CONFIGURAÇÃO DO TIMER
  Timer1.initialize(); //INICIALIZA O TIMER, PERIODO PADRAO 1000 us
  Timer1.pwm(10, 512, 1000); // CONFIGURA O PWM NA PORTA 10 COM 50%
  // ======================
}

void loop() {
  long valVai;
  long valBobMaisFreio = (valBob + freio);

  botao1.button_loop();
  botao2.button_loop();

  if (botao1.pressed()) {
    tempo = tempo + 10;
  }

  if (botao2.pressed()) {
    tempo = tempo - 10;
  }

  if (ajst > 500) {
    if ((millis() - delay1) > tempo) {
      if (freiar <= contUp) { // caso seja menor ou igual a 350 no potenciometro execute freio
        freio = freio + 1;
      }

      if (freiar >= contDw) { //caso seja maior ou igual a 500 no potenciometro execute freio
        freio = freio - 1;
      }
      delay1 = millis();
    }
  }
  if ((digitalRead(HallDig1)) == LOW) { // se o botão estiver apertado inverte o sentido da rotação
    /* Inverte o sentido de rotação */
    digitalWrite(vaiDir, LOW);
  }
  if ((digitalRead(HallDig2)) == LOW) { // se o botão estiver apertado inverte o sentido da rotação
    /* Inverte o sentido de rotação */
    digitalWrite(vaiDir, HIGH);
  }

  if ((millis() - delay3) > 5000) {
    //Serial.println(ajst);
    lcd.print(tempo);  // mostrará o valor lido no Potenciometro
    if (tempo < 100) {
      lcd.setCursor (2, 1);
      lcd.print(" ");
    }
    lcd.setCursor(4, 1);
    if (ajst > 500) {
      lcd.print("On");
      lcd.setCursor(6, 1);
      lcd.print (" ");
    } else {
      lcd.print("Off");
    }
    lcd.setCursor(8, 1);
    lcd.print (freio);  // Valor que será gravado na trava inferior
    lcd.setCursor(13, 1);
    lcd.print (freiar);
    lcd.setCursor(0, 1);
    delay3 = millis();
  }


// CONTROLE DO MOTOR DA BOBINA  
  valBob = analogRead(potBob);
  freiar = analogRead(potFreio);
  
  digitalWrite(bobPin, HIGH);
  delayMicroseconds(valBobMaisFreio);
  
  ajst   = analogRead(potAjst);
  valVai = analogRead(potPin);
  
  digitalWrite(bobPin, LOW);
  delayMicroseconds(valBobMaisFreio);
// ============================
  
  
// CONTROLE DO MOTOR SEGUNDARIO
  Timer1.setPeriod(max(valVai * 30, 800)); // MINIMO = 800uS APROX.
}
