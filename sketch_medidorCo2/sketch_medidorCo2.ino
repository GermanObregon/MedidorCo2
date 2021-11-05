#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <SPI.h>
#include <SD.h>

File archivo;
const byte filas = 4;
const byte columnas = 4;
char keys[filas][columnas] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte pinesFilas[filas] = {9,8,7,6};
byte pinesColumnas[columnas] = {5,4,3,2};
Keypad teclado = Keypad(makeKeymap(keys),pinesFilas,pinesColumnas,filas,columnas);
char tecla;
char clave[7];
char clave_default[7] = "123456";
byte indice = 0;
LiquidCrystal_I2C lcd (0x27, 2, 1, 0, 4, 5, 6, 7);
int cantidadMov = 0;
int stringStart = 0 ;
int stringStop = 16;
int s_Mq135;
int contador = 300;
const float constanteA = 5.5973021420;
const  float constanteB = -0.365425824;
float Ro = 350631.96;


void setup() {
  Serial.begin(9600);
  Serial.print("Initializing SD card...");
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");
  lcd.setBacklightPin(3 , POSITIVE);
  lcd.setBacklight(HIGH);
  
  lcd.begin(16,2);
  lcd.clear();

}

float CalcularRs(){
  float RsMedia;
  float Rs = 0;
  
  while(contador != 0){
    int adc = analogRead(0);
    Rs = Rs + 1024.0 * (20000.0/adc) - 20000.0;
    contador--;
    lcd.setCursor(0,1);
    lcd.print("Tiempo " + String(contador) + " seg. ");
    
    delay(1000);
    }
    lcd.setCursor(0,1);
    lcd.print("                ");
    contador = 300;
    RsMedia = Rs / contador;
    return RsMedia;
  
  }
float CalcularRo(){
  float Rs = CalcularRs();
  double constante = pow(416.96 , constanteB);
  Ro = Rs/(constanteA * constante);
    
  }

void Calibracion(){
  ImprimirEnPantalla("                ");
  ImprimirEnPantalla("Calibrando... ");
  CalcularRo();
  ImprimirEnPantalla("Terminado!   ");
  delay(2000);

  }
float CalcularPPM_Co2(){
  int adc = analogRead(0);
  float Rs = 1024.0 * (20000.0/adc) - 20000.0;
  float RS_RO = Rs / Ro;
  //Serial.println(RS_RO);
  float PPM = RS_RO / constanteA;
  PPM = pow(PPM , (1.0/constanteB));
  
  return PPM;
  }
void ImprimirEnPantalla(String men){
    if(cantidadMov != men.length() - 16){
      cantidadMov = men.length() - 16;
      }
    
    if(men.length()<=16){
      lcd.setCursor(0,0);
      lcd.print("");
      lcd.print(men);
    }
    else if (men.length() > 16) {
      lcd.setCursor(0,0);
      lcd.print(men.substring(stringStart,stringStop));
      delay(1000);
      lcd.clear();
    }
}
void ObtenerYGuardar(){
  
  
  for (int i = 0 ; i <= 120 ; i++){
    float ppm =CalcularPPM_Co2();
    
    Serial.println(ppm);
    
    archivo = SD.open("datos.txt",FILE_WRITE);
    archivo.print(String(i));
    archivo.print(",");
    archivo.println(String(int(ppm)));
    archivo.close();
    ImprimirEnPantalla("                ");
    ImprimirEnPantalla(String(ppm)+" PPM CO2");
    lcd.setCursor(0,1);
    lcd.print("REC " + String(i) + " muestras");
        
    delay(1000);
    lcd.print("                ");
    
    
  }
  lcd.setCursor(0,1);
  lcd.print("                ");
  
  
  
  }
void menu(){
  tecla = teclado.getKey();
  if(tecla){
    if (tecla == 'A'){
      Calibracion();    
    }
    if (tecla == 'B'){
      ObtenerYGuardar();    
    }
  }
}

void obtenerMedidaCo2(){
    float ppm =CalcularPPM_Co2();
    Serial.println(ppm);
    ImprimirEnPantalla("                ");
    ImprimirEnPantalla(String(ppm)+" PPM CO2");
    
    delay(1000);
  }
   
void loop() {
  menu();
  obtenerMedidaCo2();
 
 

}
