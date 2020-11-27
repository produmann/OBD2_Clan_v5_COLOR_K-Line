/*
назначение пинов:

serial1 > l9637d k-line


подключение SD:
cs -> 53
mosi -> 51
sck -> 52
miso -> 50
vcc > 22 через управляющий транзистор на +5в

подключение экрана:
LED > 11 через транзистор, подсветка 3,3в
SCK > 52
SDA > 51
A0  > 9
RESET>8
CS  > 10
GND > минус
VCC > 22 через управляющий транзистор на +5в

кнопка1 > 2
кнопка2 > 3
вход от подсветки > 6 (делитель 2.4Ком земля, 5.6Ком вход)
вход вкл зажигание > 7 (делитель 2.4Ком земля, 5.6Ком вход)
выход подключение динамика > 46
выход питание регистратора > 47
вход Бензобак > A0 (делитель 33Ком земля, 10Ком бак через диод)
вход Вольтметр к 12в > A1 (делитель 33Ком земля, 100Ком +)
выход кн. круиза accel > 40 через оптрон (см схему далее)
выход кн. круиза decel > 42 через оптрон (см схему далее)
вход лампа круиза > 38 через диод              к минусу лампе круиза
*/
//////////////////////// Настройка1 быстрого analogread/////////////////////////////////
#define FASTADC 1
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif
///////////////////////////////////////////////////////////////////////////////////////
#include <CyberLib.h>
#include <SPI.h>
#include <SdFat.h>
#include <TFT_ILI9163C.h>
#include <avr/pgmspace.h>
#include "_fonts/rus.c"
#include "_fonts/rus_small.c"
#include "_fonts/digit.c"
#include "Arduino.h"
#include "OBD9141.h"
#define RX_PIN 19
#define TX_PIN 18
#define __CS 10
#define __DC 9
#define __SDCS 53
#define BUFFPIXEL 20

OBD9141 obd;


boolean SDInited = true;
TFT_ILI9163C tft = TFT_ILI9163C(__CS, __DC, 8);
SdFat SD;
File myFile;
/////// текствые константы //////////////////
const char text[] PROGMEM ="РАСХОД ЗА";
const char text1[] PROGMEM ="ПОЕЗДКУ";
const char text2[] PROGMEM ="потрачено денег: ";
const char text3[] PROGMEM ="израсх. бензина:";
const char text4[] PROGMEM = "пройдено км: ";
const char text5[] PROGMEM = "расход л/100км:";
const char text6[] PROGMEM = "РАСХОД";
const char text7[] PROGMEM = "ОБЩИЙ";
const char text8[] PROGMEM = "БЕНЗОБАК";
const char text9[] PROGMEM = "в баке литров";
const char text10[] PROGMEM = "остаток пути:";
const char text11[] PROGMEM = "р.               ";
const char text12[] PROGMEM = "л.          ";
const char text13[] PROGMEM = "           ";
const char text14[] PROGMEM = "КРУИЗ";
const char text15[] PROGMEM = "температура,";
const char text16[] PROGMEM = "напряжение";
const char text17[] PROGMEM = "температ. двигателя";
const char text18[] PROGMEM = "температура впуска";
const char text19[] PROGMEM = "напряжение акб";
const char text20[] PROGMEM = "топливные";
const char text21[] PROGMEM = "коррекции";
const char text22[] PROGMEM = "долгосрочная";
const char text23[] PROGMEM = "краткосрочная";
const char text24[] PROGMEM = "ДРУГИЕ";
const char text25[] PROGMEM = "ДАТЧИКИ";
const char text26[] PROGMEM = "остальное:";
const char text27[] PROGMEM = "НАГРУЗКА:";
const char text28[] PROGMEM = "ОБОРОТЫ ДВС:";
const char text29[] PROGMEM = "МАФ СЕНСОР:";
const char text30[] PROGMEM = "ТОПЛИВНЫЙ ЦИКЛ:";
const char text31[] PROGMEM = "ДАВЛЕНИЕ ВПУСКА:";
const char text32[] PROGMEM = "УГОЛ ЗАЖИГАНИЯ:";
const char text33[] PROGMEM = "ДРОССЕЛЬ:";
const char text34[] PROGMEM = "ОШИБКИ";
const char text35[] PROGMEM = "ДВИГАТЕЛЯ";
const char text36[] PROGMEM = "основные";
const char text37[] PROGMEM = "ошибки";
const char text38[] PROGMEM = "НЕТ ОШИБОК";
const char text39[] PROGMEM = ">     ";
const char text40[] PROGMEM = "    <";
const char text41[] PROGMEM = "      стереть: кн1+кн2      ";
const char text42[] PROGMEM = "выключить двигатель!!!";
const char text43[] PROGMEM = "результаты";
const char text44[] PROGMEM = "НАСТРОЙКИ";
const char text45[] PROGMEM = "настройки:";
const char text46[] PROGMEM = "выбор/сохр.: кн1+кн2";
const char text47[] PROGMEM = "цена бензина: ";
const char text48[] PROGMEM = "сигнал скорости: ";
const char text49[] PROGMEM = "корр. скорости: ";
const char text50[] PROGMEM = "ОШИБКА ->SD";
const char text51[] PROGMEM = "вкл   ";
const char text52[] PROGMEM = "выкл   ";
const char text53[] PROGMEM = "235/75";
const char text54[] PROGMEM = "245/75";
const char text55[] PROGMEM = "отсрочка рег: ";
const char text56[] PROGMEM = "всегда вкл   ";
const char text57[] PROGMEM = "через час    ";
const char text58[] PROGMEM = "3 часа       ";
const char text59[] PROGMEM = "выкл         ";
const char text60[] PROGMEM = "калибровка бензобака";
const char text61[] PROGMEM = "МАКС значение 100%";
const char text62[] PROGMEM = "МИН значение 0%";
const char text63[] PROGMEM = "текущее значение: ";
const char text64[] PROGMEM = "сохранить: кн1+кн2";
const char text65[] PROGMEM = "ОШИБКА";
const char text66[] PROGMEM = "ВСТАВЬТЕ SD";
const char text67[] PROGMEM = "ОШИБКА SD->";
const char text68[] PROGMEM = "время простоя: ";
const char text69[] PROGMEM = "потрачено: ";
const char text70[] PROGMEM = "время езды: ";
const char text71[] PROGMEM = "GSM охрана";
const char text72[] PROGMEM = "УРОВЕНЬ СИГНАЛА:";
const char text73[] PROGMEM = "БАЛАНС:";
const char text74[] PROGMEM = "двери:";
const char text75[] PROGMEM = "капот:";
const char text76[] PROGMEM = "зажигание:";
const char text77[] PROGMEM = "слабый удар:";
const char text78[] PROGMEM = "сильный удар:";
const char text79[] PROGMEM = "вызов:";
const char text80[] PROGMEM = "   ВКЛЮЧЕНА   ";
const char text81[] PROGMEM = "ВЫКЛЮЧЕНА";
const char text82[] PROGMEM = "вкл/выкл: кн1+кн2";
const char text83[] PROGMEM = "сброс: кн1+кн2 2сек";
const char text84[] PROGMEM = "FLASH MODE OK";
const char text85[] PROGMEM = "EXIT TO COMP";
const char text86[] PROGMEM = "СРАБОТКИ:";
const char text87[] PROGMEM = "ATI";
const char text88[] PROGMEM = "AT+CLIP=1";
const char text89[] PROGMEM = "AT+CMGF=1";
const char text90[] PROGMEM = "AT+CSCS=\"GSM\"";
const char text91[] PROGMEM = "AT+CNMI=2,2";
const char text92[] PROGMEM = "AT+ENPWRSAVE=1";
const char text93[] PROGMEM = "AT+CREG=1";
const char text94[] PROGMEM = "\nSPISOK KOMAND:\nANTENNA\nAKB\nBENZ\nOFF\nON";
const char text95[] PROGMEM = ">>DVERI<<";
const char text96[] PROGMEM = ">>KAPOT<<";
const char text97[] PROGMEM = ">>ZAJIGANIE<<";
const char text98[] PROGMEM = ">>UDAR<<";
const char text99[] PROGMEM = "!!!SILNYI UDAR!!!";
const char text100[] PROGMEM = ">>VYZOV<<";
const char text101[] PROGMEM = ">>DVIZHENIE<<";
const char text102[] PROGMEM = ">>NAKLON/DVIZHENIE<<";
const char text103[] PROGMEM = "ТЕСТ";
const char text104[] PROGMEM = "акселерометр";
const char text105[] PROGMEM = "ACCEL TEST:";
const char text106[] PROGMEM = "тестов";
const char* const textMEM[] PROGMEM = {text, text1, text2, text3, text4, text5, text6, text7, text8, text9, text10, text11, 
text12, text13, text14, text15, text16, text17, text18, text19, text20, text21, text22, text23, text24, text25, text26, text27, text28, text29, 
text30, text31, text32, text33, text34, text35, text36, text37, text38, text39, text40, text41, text42, text43, text44, text45, text46, text47, text48, text49, text50,
text51, text52, text53, text54, text55, text56, text57, text58, text59, text60, text61, text62, text63, text64, text65, text66, text67,
text68, text69, text70, text71, text72, text73, text74, text75, text76, text77, text78, text79, text80, text81, text82, text83, 
text84, text85, text86, text87, text88, text89, text90, text91, text92, text93, text94, text95, text96, text97, text98, text99, text100, text101, 
text102, text103, text104, text105, text106};// создание массива текстов

const char menuTXT[] PROGMEM = "Расх.(поездка) ";
const char menuTXT1[] PROGMEM = "Расх. (общий)  ";
const char menuTXT2[] PROGMEM = "Бензин в баке  ";
const char menuTXT3[] PROGMEM = "Круиз контр.   ";
const char menuTXT4[] PROGMEM = "Температура    ";
const char menuTXT5[] PROGMEM = "Двигатель st lt";
const char menuTXT6[] PROGMEM = "Другие датч.   ";
const char menuTXT7[] PROGMEM = "Ошибки/Тест   ";
const char menuTXT8[] PROGMEM = "Настройки      ";
const char menuTXT9[] PROGMEM = "Лицензия    ";
const char* const menuTXT_MEM[] PROGMEM = {menuTXT, menuTXT1, menuTXT2, menuTXT3, menuTXT4, menuTXT5, menuTXT6, menuTXT7, menuTXT8, menuTXT9};// создание массива текстов

/////////////////////////////////////////////////////////////////////////////////
uint8_t dtc_buf[4];
char buffer[50];
String bufferSMS;
String balans;
int smsOK, ant;
byte sendcommand,sending, reg;
byte edit=0; // для редактирования =1
String var;
String val = "";
String val1 = "";
int ch = 0;
byte flashload;
double speed_korrect_val=0.99;             // Корректировка коэффициента скорости, посмотреть по GPS
double VE_correct=1.0;                     // Корректировка Объёмного КПД ДВС:  добиваемся чтобы мгновенный расход на холостых оборотах был в половину объема двигателя
double ED = 2.295;                         // Объем двигателя в литрах (пример: 1.398)
double tcorrect=0.99919;                   // Корректировка коэффициента времени 
double FuelDensityGramsPerLiter = 750.0;   // !!константа - грамм бензина в 1 литре бензина
double AirFuelRatio = 14.70;               // !!константа расхода 14,7 воздуха к 1 литра 
int bak=70;                                // сколко бак вмещает литров
int menuval=11;                             // количество элементов в основном меню
double benzstoimost = 38;                  // стоимость бензина
int fuelmin = 932;                         // уровень датчика бензина в баке 0%
int fuelmax = 221;                         // уровень датчика бензина в баке 100%
unsigned long regtimeoff=3600000;          // время выключения регистратора по умолчанию 1 час
int accel_val=2;                           // чувствительность акселерометра
bool res, D23;
bool init_success;
int init_log, kol_check_engine_error_val;
long intake_air_temp_var, speed_var, tmp_var, clERRok;
double benzraschet, LP100now, LP100, benz_potracheno, odometr, odometr_add, time, VE, raschet_nagruzka_dvigatelya_var, maf_var, long_term_val, prev_long_term_val, short_term_val, prev_short_term_val, IMAP,
 FuelFlowGramsPerSecond, FuelFlowLitersPerSecond, LPH, benz_add, dvk_var, uoz_var, polozh_dross_zaslon, benz_potrachenoMEM, odometrMEM,LP100MEM,benzraschetMEM,limitkm, LP100X, benz_stop, benz_drive, ls_term_val;
byte fss_val;
unsigned long previousMillis1, previousMillis2, previousMillis3, previousMillis4, previousMillis5, 
 previousMillis6, previousMillis7, time_new, time_old, time_old_gurnal, time_corr, time_corr_prev, 
 time_scr, time_stop, time_drive, time_inf, time_alarm, timer1, timer2;
byte selmon=0;
byte flag,flag1,flag2, flag3, flag5, par, key1, key2, key3, key1_sec, key3_2sec, menu_scr, clearErrorFN, fuelrun, prev_menu_flag, draw, drawtxt, menu0_flag, menu109_flag, inf;
byte reg_flag, alarm, alarm1, alarm1_1, alarm2, alarm2_1, alarm3, alarm3_1, alarm4, alarm4_1, alarm5, alarm5_1, alarm6, alarm6_1, alarm7, alarm7_1, alarm8, alarm8_1, cruise, cruise1, cruise2;
int alarm1_count, alarm2_count, alarm3_count, alarm4_count, alarm5_count, alarm6_count;
byte alarm_ON_OFF=1;
byte flag4=9;
byte key3_alarm;// для выключения сигналки
unsigned long alarm_timer;
String infAlarm;
int GSM_level;
byte bomj, sim_inf, gsm_flag;
double vbat;
byte speedlimit=1;
byte comp, speed_flag, speed_flag1, ring_flag, ring_flag1, alarm_flag;
int reg_on=1;
int rpm_var;
int h,h1,w,menu,prev_menu,menu_select;
int selm=1;
int m1=1;
int light;
double analog2, fvar, fuel, fuel_prev;
int analog[100];
unsigned long analog1=0;
int ii;
int sel;
String kod[2];
int accel_x, accel_y, accel_z, accel_x1, accel_y1, accel_z1, accel_test;
int point_x, point_y, point_x_prev, point_y_prev, lyambd_val,timeGraph, lyambd_val_max, lyambd_val_min;
byte time_graph_fag=1;
unsigned long time_graph;

void setup()
{ 
 analogReference(EXTERNAL);//внешний источник опорного напряжения
/////////////////////// Настройка2 быстрого analogread/////////////////////////////////
  #if FASTADC
      sbi(ADCSRA,ADPS2) ;
      cbi(ADCSRA,ADPS1) ;
      cbi(ADCSRA,ADPS0) ;
  #endif
///////////////////////////////////////////////////////////////////////////////////////
  
  Serial.begin(115200);    // для перепрошивки
  obd.begin(Serial1, RX_PIN, TX_PIN); // k-line инициализация
  D49_Out;                 // сброс для прошивки
  D2_In; D2_High;          // КНОПКА1 МЕНЮ на 2 pin с подтягивающим резистором 
  //attachInterrupt(0, key, FALLING);
  D3_In; D3_High;          // КНОПКА2 на 3 pin с подтягивающим резистором
  //attachInterrupt(1, key, FALLING);
  D22_Out;                 // вкл дисплей  pin22
  D7_In; D7_High;          // проверка ВКЛ зажигание pin7
  D6_In; D6_High;          // подсветка
  D47_Out;                 // регистратор
  D40_Out;                 // accel
  D42_Out;                 // decel
  D38_In; D38_High;        // лампа круиза через диод ->|- к минусу лампе круиза
}

void loop() 
{
   
  ////////////// включение регистратора при вкл зажигании и опрос сериал команд///////////////////////////////// 
  if(D7_Read==HIGH){
  D47_High; ///регистратор вкл
  previousMillis5 = millis();// для регистратора и задержки выкл
  }
  if(reg_on==1 || reg_on==2){//выкл регистратор через час или 3часа
   if (millis() - previousMillis5 > regtimeoff) {D47_Low;} 
   }
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////

  ////////// в режиме прошивки выключение экрана при выкл зажигании и сохранение параметров на флеш/////////////
  if(D7_Read==LOW && flashload==2) 
  {flashload=0;
  if(benz_potrachenoMEM!=0 && odometrMEM!=0){SDwrite();} 
  delay(500); D22_Low; comp=0;}
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
////////////////////////////////////////// Напряжение АКБ //////////////////////////////////////////////////////////////////////
  for(int i = 0; i < 100; i++) {
 vbat=vbat + (double(map(analogRead(1), 0, 1023, 0, 2000))/100);
 }
 if (rpm_var>400 && D7_Read==HIGH){vbat=(vbat/100);} // при вкл. двигателе
 else{vbat=(vbat/100)*1.033;} // при выкл. двигателе
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////БОРТОВОЙ КОМПЬЮТЕР/////////////////////////////////////////////////////////////



////////////////////////////////////////////// ЗАЖИГАНИЕ ВКЛЮЧЕНО, Запуск БК/////////////////////////////
if(D7_Read==HIGH && comp==0) 
{light=0;

////////////экран//////////////////////////////////////////////////////////////////////////////////
 D22_High;// включаем дисплей
 D35_High;// вкл gsm 
 tft.begin();// инициализация дисплея
 bmpDraw("logo.bmp", 0, 0);//отрисовка картинки на sd
 while (light<220) {light++; analogWrite(11, light); delay(10);}// включение подсветки
 //////////////////////////////////////////////////////////////////////////////////////////////////
 
//tone(46, 3000, 100); delay (100); tone(46, 3500, 100); delay (100); tone(46, 4000, 100); delay (100); tone(46, 4500, 100);
init_success =  obd.init(); // инициализация k-line
speed_var =0;
tmp_var = 0;
rpm_var = 0;
intake_air_temp_var = 0;
raschet_nagruzka_dvigatelya_var = 0;
maf_var = 0;
short_term_val = 0;
long_term_val = 0;
fss_val = 0;  
kol_check_engine_error_val=0;
kod[0]="0"; kod[1]="0"; kod[2]="0";
delay(500);  
fuelrun=0; // топливо в баке 1 раз при включении
ii=0;
comp=1;
SearchErrorEngine();// поиск ошибок
SDread();//  восстанавливаем значения с карты
tft.clearScreen();
Serial2.println("AT+CREG?");// запрос регистрации в сети
delay(100); D35_Low;// выкл gsm
previousMillis2 = millis(); // для рассчета топлива в баке  
previousMillis1 = millis();// для меню 
previousMillis4 = millis();// топливо в баке 1 раз при включении
previousMillis6 = millis();//  сброс времени простоя авто
draw=1;
drawtxt=1;
}
///////////////////////////////////////////////////////////////////////////////////////////////////



  
////////// Выключение БК//////////////////////////////////////////////////////////////////////////
if(D7_Read==LOW && (comp==1) && (millis() - previousMillis5 > 5000)) {
  while (light>0) {light--; analogWrite(11, light); delay(10);}// выключение подсветки 
  if(benz_potrachenoMEM!=0 && odometrMEM!=0){SDwrite(); delay(500);} 
  //// отключаем все порты///////
  D22_Low;
  D52_Low;
  D51_Low;
  D9_Low;
  D8_Low;
  D10_Low;
  D50_Low;
  D53_Low;
  comp=0;
  if(reg_on==3) {D47_Low;}//выкл регистратор
  }
//////////////////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////// БОРТОВОЙ КОМПЬЮТЕР/////////////////////////////////////////////////////
if ((millis() - previousMillis4 > 6000) && !fuelrun) {analog2 = analogRead(0); fuelrun=1;} /// первая проверка уровня бензина при включении зажигания
if (comp==1) // комп ВКЛ
{
if (light==220 && D6_Read==HIGH) {light=50; analogWrite(11, light);}// управление подсветкой
if (light==50 && D6_Read==LOW) {light=220; analogWrite(11, light);} // управление подсветкой
  
  IMAP=0;
  FuelFlowGramsPerSecond=0;
  FuelFlowLitersPerSecond=0;
  LPH=0;
  odometr_add=0;
  benz_add=0;
  LP100now = 0;
  ls_term_val=0;

if(D7_Read==HIGH) {fuel_var();}
if(!menu_scr && (menu == 9 || menu == 91)) {
  Maf();
} else {
 if((millis()-timer1)>900) {timer1=millis(); Rpm();} // задержка опроса
 if((millis()-timer2)>500) {timer2=millis(); Rnd();} // задержка опроса
 Speed();
 Maf();
 FuelSystemStatus();
 LongTerm();
 if (fss_val==2) {ShortTerm();} else {short_term_val=0;}
}


  if(speedlimit){
   if(speed_var>80 && speed_flag == 0)   // пищит при превышении скорости
     {if(!prev_menu_flag) {prev_menu=menu;}
     menu_scr=0; menu=110; previousMillis1 = millis(); 
      speed_flag = 1;  tone(46, 4500, 500); draw=1;} 
   if(speed_var<60 && speed_flag == 1) { speed_flag = 0;} 
   
   if(speed_var>100 && speed_flag1 == 0)   // пищит при превышении скорости
     {if(!prev_menu_flag) {prev_menu=menu;}
     menu_scr=0; menu=111; previousMillis1 = millis();
      speed_flag1 = 1;  tone(46, 4500, 500); draw=1;} 
   if(speed_var<80 && speed_flag1 == 1) { speed_flag1 = 0;}
  }

/////////////// ОСНОВНЫЕ ВЫЧИСЛЕНИЯ//////////////////     
if (rpm_var>400 && D7_Read==HIGH) // проверка работы двигателя 
{  if (raschet_nagruzka_dvigatelya_var<20 && fss_val==4) {} // Проверка торможения двигателем по нагрузке двигателя и FSS
     else
     {if (fss_val==2) {   // если замкнутая обратная связь  - Closed Loop
        ls_term_val=double(100.0+(long_term_val+short_term_val))/100.0; // коэффициент корректировки расхода по ShortTerm и LongTerm    
      } 
      else {
        ls_term_val=double(100.0+long_term_val)/100.0; // коэффициент корректировки расхода по LongTerm  
      }
      FuelFlowGramsPerSecond = double(maf_var/AirFuelRatio)*ls_term_val;   // Получаем расход грамм бензина в секунду в соотношении 14,7 воздуха/к 1 литра бензина, корректировка ls_term_val
      FuelFlowLitersPerSecond = FuelFlowGramsPerSecond / FuelDensityGramsPerLiter;  // Переводим граммы бензина в литры
      LPH = FuelFlowLitersPerSecond * 3600.0;}       // Конвертирование литров в час
      
    if (time_old==0) {time_old = millis();}  // выполнится один раз при появлении оборотов    

    time_new = millis();   // время со старта программы в мс
    time=(double(time_new-time_old)/1000.0)*tcorrect;     // прошло время с последнего расчета скорости, расхода  - в сек
     if (time>10) {  time=0; }
    time_old = time_new;  // записать новое время для сравнения в следующем цикле

if (speed_var>0)  
      {odometr_add=double((double(speed_var*1000.0)/3600.0)*time)/1000.0;
       odometr=odometr+odometr_add;}    //обший пробег в км
       
    benz_add=FuelFlowLitersPerSecond*time;
    benz_potracheno=benz_potracheno+benz_add;  // общий расход в литрах
}
  if (odometr>0) {LP100=(benz_potracheno/odometr)*100.0;} //расход бензина на 100 км (в литрах)     
  if (speed_var>0) {LP100now = (benz_add/odometr_add)*100.0;} // литров/100км мгновенное
  benzraschet = benz_potracheno*benzstoimost;
  
  benz_potrachenoMEM=benz_potrachenoMEM+benz_add;// общее 
  odometrMEM=odometrMEM+odometr_add;// общее 
  if (odometrMEM>0) {LP100MEM=(benz_potrachenoMEM/odometrMEM)*100.0;} //ОБЩИЙ расход бензина на 100 км (в литрах)
  benzraschetMEM = benz_potrachenoMEM*benzstoimost;//ОБЩЕЕ
  if(LP100>0) {LP100X=LP100;}// расход для расчетов сколько осталось проехать
  else {LP100X=LP100MEM;}
  if (LP100X==0) {limitkm=0;}
  else {limitkm=(fuel/LP100X)*100;} // сколько можно проехать на остатке топлива

  if (speed_var<=5 && rpm_var>400) {//сохраняем время остановки в секундах
   time_stop = time_stop+(millis()-previousMillis6);
   benz_stop = benz_stop+benz_add;
  } 
  if (speed_var>5 && rpm_var>400) {//сохраняем время движения в секундах
   time_drive = time_drive+(millis()-previousMillis6);
   benz_drive = benz_drive+benz_add;
  }
  previousMillis6 = millis();// время для рассчета
 
 /////// КОНЕЦ ОБРАБОТКИ, ВЫВОД НА ЭКРАН////////
 
 if(clearErrorFN){clearError(); // сброс ошибок двигателя....
  if(clERRok==1){tone(46, 4500, 50); delay (100); tone(46, 4500, 50); delay (100); tone(46, 4500, 50); previousMillis1 = millis(); menu=107; draw=1; kol_check_engine_error_val=0;
  kod[0]="0"; kod[1]="0"; kod[2]="0";}
  else {tone(46, 800, 200); bmpDraw("err.bmp", 6, 27); menu=6; delay (800); draw=1; tft.clearScreen();}
  clearErrorFN=0;}
  
 if(!menu_scr) { 
////опрос датчиков/////
 if(menu == 3) {Tmp(); IntakeAirTemp();}
 if(menu == 5) {Uoz(); DavlenVpuskKoll(); Pdz();}
 if(menu == 6 && flag4==9) {SearchErrorEngine();}
 if(menu == 6 && !flag5) {error03();}
 if(menu == 61 && !flag5) {error07();}
 if(menu == 9) {Oxygen1();}
 if(menu == 91) {Oxygen2();}
 }
 key(); // обработчик кнопок
 LCD();
}
 
 if (init_log>10 && D7_Read==HIGH) {init_success =  obd.init(); init_log=11;}// если нет соединения, инициализация
}


////////////////////////////////////////////////////// ПОДПРОГРАММЫ ////////////////////////////////////////////////////////////////////////////
void LCD(void){// ОТРИСОВКА ЭКРАНА
if(alarm){// в режиме охраны отображается только скорость
     tft.setFont(&digit);//выбор шрифта
     tft.setTextColor(WHITE, BLACK);
     if (speed_var<10) {tft.setCursor(20,28);  tft.print("   "); tft.print(speed_var); tft.print("  ");}
     if (speed_var>=10 && speed_var<100) {tft.setCursor(20,28);  tft.print("  "); tft.print(speed_var); tft.print(" ");}
     if (speed_var>=100) {tft.setCursor(20,28); tft.print(speed_var);}
     tft.setFont(&rus);//выбор шрифта
     tft.setTextColor(YELLOW, BLACK);
     tft.setCursor(48,73);
     tft.print("км/ч");
     draw=101;}
else{
  
  if(key1_sec && menu_scr) {menu_scr=0; key1_sec=0; key1=0; draw=1; previousMillis1 = millis(); tft.clearScreen();}// выход на экран
  if(key1_sec && !menu_scr) {menu_scr=1; key1_sec=0; key1=0; draw=1; drawtxt=1; tft.clearScreen(); flag4=9;}// выход в основное меню
  ///////////////круиз выход
  if(menu==8 && key3 && D38_Read==LOW && !menu_scr) {menu_scr=1; key1_sec=0; key1=0; draw=1; drawtxt=1; tft.clearScreen();
  }
  ////////////отрисовка основного меню////////////////////////////////////////////////////////
  if(menu_scr) {
  if(key2) {selm++; menu_select++; drawtxt=1;}
  if(selm>4) {m1++; selm=4;}
  if(key1) {selm--; menu_select--; drawtxt=1;}
  if(selm<1) {m1--; selm=1;}
  if(menu_select<0) {selm=1; m1=1; menu_select=0; tone(46, 4500, 50); delay(100); tone(46, 4500, 50);}
  if(menu_select>(menuval-1)) {selm=4; m1=(menuval-3); menu_select=(menuval-1); tone(46, 4500, 50); delay(100); tone(46, 4500, 50);}
  if(menu_select==0) {menu=0;} // пункт-0 >>> экран-0
  if(menu_select==1) {menu=1;} // пункт-1 >>> экран-1
  if(menu_select==2) {menu=2;} // пункт-2 >>> экран-2
  if(menu_select==3) {menu=8;} // пункт-3 >>> экран-8
  if(menu_select==4) {menu=3;} // пункт-4 >>> экран-3
  if(menu_select==5) {menu=4;} // пункт-5 >>> экран-4
  if(menu_select==6) {menu=9;} // пункт-6 >>> экран-9
  if(menu_select==7) {menu=5;} // пункт-7 >>> экран-5
  if(menu_select==8) {menu=6;} // пункт-8 >>> экран-6
  if(menu_select==9) {menu=7;} // пункт-9 >>> экран-7
  if(menu_select==10) {menu=999;} // пункт-9 >>> экран-7
  

  if(draw==1) {// отрисовка заголовка
  bmpDraw("menu.bmp", 0, 0);//отрисовка картинки на sd
  draw=0;}
  if(drawtxt) {// отрисовка меню

 /////////// текст //////////////////////////// 
  tft.setFont(&rus);//выбор шрифта
  
  h1=25*((m1-2)*(-1)); if(h1>=25 && h1<=100) {
    if(menu_select==0) {tft.setTextColor(YELLOW, BLACK);}
     else {tft.setTextColor(WHITE, BLACK);}
    strcpy_P(buffer, (char*)pgm_read_word(&(menuTXT_MEM[0]))); // вывод из flash текста
    tft.setCursor(3,h1);  tft.println(buffer); //tft.println("Расх.(поездка) ");
    }
  h1=25*((m1-3)*(-1)); if(h1>=25 && h1<=100) {
    if(menu_select==1) {tft.setTextColor(YELLOW, BLACK);}
     else {tft.setTextColor(WHITE, BLACK);}
    strcpy_P(buffer, (char*)pgm_read_word(&(menuTXT_MEM[1]))); // вывод из flash текста 
    tft.setCursor(3,h1);  tft.println(buffer); //tft.println("Расх. (общий)  ");
    }
  h1=25*((m1-4)*(-1)); if(h1>=25 && h1<=100) {
    if(menu_select==2) {tft.setTextColor(YELLOW, BLACK);}
     else {tft.setTextColor(WHITE, BLACK);}
    strcpy_P(buffer, (char*)pgm_read_word(&(menuTXT_MEM[2]))); // вывод из flash текста
    tft.setCursor(3,h1);  tft.println(buffer); //tft.println("Бензин в баке  ");
    }
  h1=25*((m1-5)*(-1)); if(h1>=25 && h1<=100) {
    if(menu_select==3) {tft.setTextColor(YELLOW, BLACK);}
     else {tft.setTextColor(WHITE, BLACK);}
    strcpy_P(buffer, (char*)pgm_read_word(&(menuTXT_MEM[3]))); // вывод из flash текста
    tft.setCursor(3,h1);  tft.println(buffer); //tft.println("Круиз контроль        ");
    }
  h1=25*((m1-6)*(-1)); if(h1>=25 && h1<=100) {
    if(menu_select==4) {tft.setTextColor(YELLOW, BLACK);}
     else {tft.setTextColor(WHITE, BLACK);}
    strcpy_P(buffer, (char*)pgm_read_word(&(menuTXT_MEM[4]))); // вывод из flash текста
    tft.setCursor(3,h1);  tft.println(buffer); //tft.println("Температура    ");
    }
  h1=25*((m1-7)*(-1)); if(h1>=25 && h1<=100) {
    if(menu_select==5) {tft.setTextColor(YELLOW, BLACK);}
     else {tft.setTextColor(WHITE, BLACK);}
    strcpy_P(buffer, (char*)pgm_read_word(&(menuTXT_MEM[5]))); // вывод из flash текста
    tft.setCursor(3,h1);  tft.println(buffer); //tft.println("Двигатель st lt");
    }
    h1=25*((m1-8)*(-1)); if(h1>=25 && h1<=100) {
    if(menu_select==6) {tft.setTextColor(YELLOW, BLACK);}
     else {tft.setTextColor(WHITE, BLACK);}
    tft.setCursor(3,h1); tft.println("Лямбда тест       ");
    }
  h1=25*((m1-9)*(-1)); if(h1>=25 && h1<=100) {
    if(menu_select==7) {tft.setTextColor(YELLOW, BLACK);}
     else {tft.setTextColor(WHITE, BLACK);}
    strcpy_P(buffer, (char*)pgm_read_word(&(menuTXT_MEM[6]))); // вывод из flash текста
    tft.setCursor(3,h1);  tft.println(buffer); //tft.println("Другие датч.   ");
    }
  h1=25*((m1-10)*(-1)); if(h1>=25 && h1<=100) {
    if(menu_select==8) {tft.setTextColor(YELLOW, BLACK);}
     else {tft.setTextColor(WHITE, BLACK);}
    strcpy_P(buffer, (char*)pgm_read_word(&(menuTXT_MEM[7]))); // вывод из flash текста
    tft.setCursor(3,h1);  tft.println(buffer); //tft.println("Ошибки         ");
    }
  h1=25*((m1-11)*(-1));if(h1>=25 && h1<=100) {
    if(menu_select==9) {tft.setTextColor(YELLOW, BLACK);}
     else {tft.setTextColor(WHITE, BLACK);}
    strcpy_P(buffer, (char*)pgm_read_word(&(menuTXT_MEM[8]))); // вывод из flash текста
    tft.setCursor(3,h1);  tft.println(buffer); //tft.println("Настройки      ");
    }
  h1=25*((m1-12)*(-1));if(h1>=25 && h1<=100) {
    if(menu_select==10) {tft.setTextColor(YELLOW, BLACK);}
     else {tft.setTextColor(WHITE, BLACK);}
    strcpy_P(buffer, (char*)pgm_read_word(&(menuTXT_MEM[9]))); // вывод из flash текста
    tft.setCursor(3,h1);  tft.println(buffer); 
    }
  ///////////////////////////////////////////////////
 
 ///////// выбор пункта (рамка)  /////////////
 tft.drawRect(0,22,128,26,BLACK);
 tft.drawRect(0,47,128,26,BLACK);
 tft.drawRect(0,72,128,26,BLACK);
 tft.drawRect(0,97,128,26,BLACK);
 if(selm==1) {tft.drawRect(0,22,128,26,YELLOW);}
 if(selm==2) {tft.drawRect(0,47,128,26,YELLOW);}
 if(selm==3) {tft.drawRect(0,72,128,26,YELLOW);}
 if(selm==4) {tft.drawRect(0,97,128,26,YELLOW);}
 /////////////////////////////////////////////

 //////////стрелочки/////////////////////////
 if (m1>1) {tft.fillTriangle(69, 127, 75, 127, 72, 124, YELLOW);}//отрисовка стрелок 
  else {tft.fillRect(69,124,7,4,BLACK);}
 if (m1<(menuval-3)) {tft.fillTriangle(53, 124, 59, 124, 56, 127, YELLOW);}//отрисовка стрелок
  else {tft.fillRect(53,124,7,4,BLACK);} 
 /////////////////////////////////////////////
 drawtxt=0;
   } 
   sim_inf=0;
 }
//////////////////////////////////////////////////////////////////////////////


///////////////ПОДМЕНЮ экранов функция>>(menu)//////////////////////////////////////////////////////////////
if(!menu_scr) { 
    
 if(menu==0 && (millis() - previousMillis3 > 1500) && flag1) { // сброс поездки
  LP100=0; benz_potracheno=0; time_stop=0; time_drive=0; benz_stop=0; benz_drive=0;
  odometr=0; tone(46, 4500, 50); delay (100); tone(46, 4500, 50); delay (100); tone(46, 4500, 50);}
  if(menu==0 && !menu0_flag && key1) {tone(46, 4500, 50); delay(100); tone(46, 4500, 50);}
  if(menu==0 && menu0_flag && key2 && (millis() - previousMillis3 < 1500))  {tone(46, 4500, 50); delay(100); tone(46, 4500, 50);}
  if(menu==0 && menu0_flag && key1)  {draw=1; menu0_flag=0;}
  if(menu==0 && !menu0_flag && key2 && (millis() - previousMillis3 < 1500)) {draw=1; menu0_flag=1;}
  
 if(menu==1 && key3_2sec==1) {menu=101; previousMillis1 = millis(); draw=1;} // меню обнулить общие счетчики двойное нажатие 2 сек
 if(menu==101 && key1) { // обнуление....
  benz_potrachenoMEM=0; odometrMEM=0;
  LP100MEM=0; SDwrite();
  menu=1; tone(46, 4500, 50); delay (100); tone(46, 4500, 50); delay (100); tone(46, 4500, 50);
  draw=1; tft.clearScreen();}
 if(menu==101 && key2) {menu=1; draw=1; tft.clearScreen();} // отмена, обратно в меню

if(menu==61 && key2) {tone(46, 4500, 50); delay(100); tone(46, 4500, 50);}
if(menu==6 && key2) {menu=61; flag5=0; draw=1;} // переключение режима ошибок
if(menu==6 && key1) {tone(46, 4500, 50); delay(100); tone(46, 4500, 50);}////////////////////
if(menu==61 && key1) {menu=6; flag5=0; draw=1;} // переключение режима ошибок
if((menu==6 || menu==61) && key3_2sec==1 && rpm_var==0) {menu=106; previousMillis1 = millis(); draw=1;}  // меню стереть ошибки двойное нажатие 2 сек
if(menu==106 && key1) {clearErrorFN=1;}
 if(menu==106 && key2) {menu=6; flag5=0; draw=1; tft.clearScreen();} // отмена, обратно в меню

if(menu==7 && key2 && par==0) {sel++; draw=1;}// листание параметров
if(menu==7 && key1 && par==0) {sel--; draw=1;}// листание параметров
if(menu==7 && sel<0) {sel=0; tone(46, 4500, 50); delay(100); tone(46, 4500, 50);}
if(menu==7 && sel>4) {sel=4; tone(46, 4500, 50); delay(100); tone(46, 4500, 50);}
if(menu==7 && key3 && par==0) {par=1; draw=1;}// выбор параметров
if(menu==7 && key3 && par==2) {// сохранение параметров
  par=0; if(benz_potrachenoMEM!=0 && odometrMEM!=0){SDwrite(); } 
  tone(46, 4500, 50); delay (100); tone(46, 4500, 50); delay (100); tone(46, 4500, 50); draw=1;}
  ////////////////////цена бензина//////////////////////////
if(menu==7 && sel==0 && (par==1 || par==2) && key1) // изменение значения
    {benzstoimost=benzstoimost+0.1; draw=1;}
if(menu==7 && sel==0 && (par==1 || par==2) && key2) // изменение значения
    {benzstoimost=benzstoimost-0.1; draw=1;}
   ////////////////////коррекция скорости//////////////////////////
if(menu==7 && sel==2 && (par==1 || par==2) && key1) // изменение значения
    {speed_korrect_val=0.99; draw=1;}
if(menu==7 && sel==2 && (par==1 || par==2) && key2) // изменение значения
    {speed_korrect_val=1.02;  draw=1;}  
    /////////////////// сигнал скорости/////////////////////
if(menu==7 && sel==1 && (par==1 || par==2) && (key2 || key1)) // изменение значения
    {speedlimit=!speedlimit; draw=1;}
  ////время регистратора////  
if(menu==7 && sel==3 && (par==1 || par==2) && key1) // изменение значения время регистратора
    {reg_on++; draw=1;}
if(menu==7 && sel==3 && (par==1 || par==2) && key2) // изменение значения время регистратора
    {reg_on--; draw=1;}    
if(reg_on<0) {reg_on=0; tone(46, 4500, 50); delay(100); tone(46, 4500, 50);} 
if(reg_on>3) {reg_on=3; tone(46, 4500, 50); delay(100); tone(46, 4500, 50);} 
if(reg_on==1) {regtimeoff=3600000;}  
if(reg_on==2) {regtimeoff=10800000;}
    ////подменю корректировка топливного бака////
if(menu==7 && sel==4 && key3) {menu=108; tft.clearScreen(); draw=1;}
if(menu==108 && key1) {fuelmax=analog2; draw=1;} //запись значения датчика бензина 100%
if(menu==108 && key2) {fuelmin=analog2; draw=1;} //запись значения датчика бензина 0%
if(menu==108 && key3_2sec) {fuelmin = 932; fuelmax = 221; draw=1;} // значения датчика бензина по умолчанию кнопка1 + кнопка2 2секунды
if(menu==108 && key3 && par==0) {menu=7; draw=2;// сохранение параметров кнопка1 + кнопка2
  if(benz_potrachenoMEM!=0 && odometrMEM!=0){SDwrite();} 
  tone(46, 4500, 50); delay (100); tone(46, 4500, 50); delay (100); tone(46, 4500, 50);}
 
//////////////////////////////////////Лямбда////////////////////////////////////////////////////////
if(menu==91 && key2) {tone(46, 4500, 50); delay(100); tone(46, 4500, 50);}
if(menu==9 && key2) {menu=91; draw=2;} // переключение лямбды
if(menu==9 && key1) {tone(46, 4500, 50); delay(100); tone(46, 4500, 50);}
if(menu==91 && key1) {menu=9; draw=2;} // переключение лямбды
if((menu==91 || menu==9) && key3) {time_graph_fag=!time_graph_fag; draw=2;} // переключение времени графика

////////////////////////////// ПРОРИСОВКА ЭКРАНА/////////////////////////////////////////
if(D7_Read==HIGH) {  
  if (init_success || edit){
    if (draw==101) {tft.clearScreen(); draw=1;}
switch (menu){
  case 0:
  ////////////ЗА ПОЕЗДКУ, МГНОВЕННЫЙ РАСХОД, ИЗРАСХОДОВАНО ДЕНЕГ, СРЕДНИЙ РАСХОД...../////////////////////////////////////////////////////
  if (millis() - previousMillis1 < 1800) {
     if(draw==1) {// отрисовка заголовка
      bmpDraw("menu0_1.bmp", 0, 0);//отрисовка картинки на sd
      tft.setFont(&rus);
      tft.fillRect(0,41,128,45,RED);
      tft.setTextColor(BLACK);
      strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[0]))); // вывод из flash текста
      tft.setCursor(16,43); tft.print(buffer); //tft.print("РАСХОД ЗА");
      strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[1]))); // вывод из flash текста
      tft.setCursor(26,63); tft.print(buffer); //tft.print("ПОЕЗДКУ");
      tft.setTextColor(WHITE);
      strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[0]))); // вывод из flash текста
      tft.setCursor(15,42); tft.print(buffer); //tft.print("РАСХОД ЗА");
      strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[1]))); // вывод из flash текста
      tft.setCursor(25,62); tft.print(buffer); //tft.print("ПОЕЗДКУ");
      bmpDraw("menu0_2.bmp", 0, 86);//отрисовка картинки на sd
     draw=2;}   
         }
    else {
     if(draw==2) {// очистка экрана
     tft.clearScreen();
     draw=1;} 
     if(draw==1) {// отрисовка экрана
     tft.drawRect(0,0,128,55,RED);
     tft.fillRect(0,75,128,53,BLACK);
     tft.setFont(&rus_small);//выбор шрифта
     tft.setCursor(0,76); 
     if(!menu0_flag){
     tft.setTextColor(WHITE, BLACK);
     
     for (int i = 2; i < 6; i++)// вывод из flash текста
  {
   strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[i]))); // вывод из flash текста 
   tft.println(buffer);
  }
    /* 
     tft.println("потрачено денег:");
     tft.println("израсх. бензина:");
     tft.println("пройдено км:");
     tft.println("расход л/100км:");*/
     } else {
     tft.setTextColor(YELLOW, BLACK);
     strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[68]))); // вывод из flash текста 
     tft.println(buffer); // tft.println("время простоя: ");
     strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[69]))); // вывод из flash текста 
     tft.println(buffer); // tft.println("потрачено: ");
     tft.setTextColor(WHITE, BLACK);
     strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[70]))); // вывод из flash текста 
     tft.println(buffer); // tft.println("время езды: ");
     strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[69]))); // вывод из flash текста 
     tft.println(buffer); //tft.println("потрачено: "); 
     }
     draw=3;} 

   tft.setFont(&digit);//выбор шрифта
     if (raschet_nagruzka_dvigatelya_var<20 && fss_val==4) 
      {tft.setCursor(1,1); tft.print("  -.-- ");
       if(draw==4 || draw==5) {bmpDraw("dvs.bmp", 0, 55); draw=3;}
      }// торможение двигателем
  else{
  if (speed_var>5) {
  if(draw==4 || draw==3) {bmpDraw("l100h.bmp", 0, 55); draw=5;}//отрисовка картинки на sd
   if (LP100now<25) {tft.setTextColor(WHITE, BLACK);}
   if (LP100now>=25 && LP100now<=35) {tft.setTextColor(YELLOW, BLACK);}
   if (LP100now>35) {tft.setTextColor(RED, BLACK);}
   if (LP100now<9.99) {tft.setCursor(1,1); tft.print("  "); tft.print(LP100now);}//литр на 100км мгновенное
   if (LP100now>=9.99 && LP100now<99.9) {tft.setCursor(1,1); tft.print("  "); tft.print(LP100now,1);}
   if (LP100now>=99.9) {tft.setCursor(1,1); tft.print("  "); tft.print(LP100now,0); tft.print("  ");}
  }
  else { 
  if(draw==5 || draw==3) {bmpDraw("lh.bmp", 0, 55); draw=4;}//отрисовка картинки на sd
   tft.setTextColor(WHITE, BLACK);
   if (LPH>9.99) {tft.setCursor(1,1); tft.print("  "); tft.print(LPH,1);}//литров в час
    else {tft.setCursor(1,1); tft.print("  "); tft.print(LPH);}
   }
 
  }
  tft.setFont(&rus_small); 
  if(!menu0_flag){
  tft.fillTriangle(58, 124, 64, 124, 61, 127, YELLOW); 
  tft.setTextColor(WHITE, BLACK);
  strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[11]))); // вывод из flash текста
  tft.setCursor(88,76); tft.print(benzraschet,0);  tft.print(buffer); //tft.print("р.               ");// денег
  strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[12]))); // вывод из flash текста
  tft.setCursor(87,88); tft.print(benz_potracheno,1);  tft.print(buffer); //tft.print("л.          "); // бензин
  strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[13]))); // вывод из flash текста
  tft.setCursor(67,100); tft.print(odometr,1); tft.print(buffer); //tft.print("           ");          // одо
  tft.setCursor(82,112); tft.print(LP100,1); tft.print(buffer); //tft.print("           ");            // расход
  } else {
  tft.fillTriangle(58, 78, 64, 78, 61, 75, YELLOW);
  tft.setTextColor(YELLOW, BLACK);
  tft.setCursor(79,76); 
   time_scr=time_stop/1000;
   if (time_scr/60/60<10) { tft.print ("0"); }
     tft.print (time_scr/60/60);
     tft.print("ч"); 
   if (time_scr/60%60<10) { tft.print ("0"); }
     tft.print ((time_scr/60)%60);
     tft.print("м        "); // время простоя
  tft.setCursor(58,88); tft.print(benz_stop,1);  tft.print("л./ "); tft.print((benz_stop*benzstoimost),0); tft.print("р.     "); // потрачено бензина простой
  tft.setTextColor(WHITE, BLACK);
  tft.setCursor(65,100);
   time_scr=time_drive/1000;
   if (time_scr/60/60<10) { tft.print ("0"); }
     tft.print (time_scr/60/60);
     tft.print("ч"); 
   if (time_scr/60%60<10) { tft.print ("0"); }
     tft.print ((time_scr/60)%60);
     tft.print("м        ");// время езды
  tft.setCursor(58,112); tft.print(benz_drive, 1); tft.print("л./ "); tft.print((benz_drive*benzstoimost),0); tft.print("р.     "); // потрачено бензина за езду  
  }
    } 
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////  
  break;

  case 1:
////////////////////////ОБЩИЙ РАСХОД////////////////////////////////////////////////
 if (millis() - previousMillis1 < 1800) {
     if(draw==1) {// отрисовка заголовка
      bmpDraw("menu1_1.bmp", 0, 0);//отрисовка картинки на sd
      tft.setFont(&rus);
      tft.fillRect(0,41,128,45,BLUE);
      tft.setTextColor(BLACK);
      strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[6]))); // вывод из flash текста
      tft.setCursor(29,43); tft.print(buffer); //tft.print("РАСХОД");
      strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[7]))); // вывод из flash текста
      tft.setCursor(31,64); tft.print(buffer); //tft.print("ОБЩИЙ");
      tft.setTextColor(WHITE);
      strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[6]))); // вывод из flash текста
      tft.setCursor(28,42); tft.print(buffer); //tft.print("РАСХОД");
      strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[7]))); // вывод из flash 
      tft.setCursor(30,63); tft.print(buffer); //tft.print("ОБЩИЙ");
      bmpDraw("menu1_2.bmp", 0, 86);//отрисовка картинки на sd
     draw=2;}   
         }
    else {
     if(draw==2) {// очистка экрана
     tft.clearScreen();
     draw=1;}
    if(draw==1) {// отрисовка экрана
     bmpDraw("menu1.bmp", 0, 0);//отрисовка картинки на sd
     draw=3;} 
    tft.setFont(&rus); tft.setTextColor(WHITE, BLACK);
    if (LP100MEM<10) {tft.setCursor(69,3); tft.print(" "); tft.print(LP100MEM); tft.print(" ");}
    if (LP100MEM>=10 && LP100MEM<100) {tft.setCursor(69,3); tft.print(" "); tft.print(LP100MEM,1); tft.print(" ");}
    if (LP100MEM>=100) {tft.setCursor(69,3); tft.print(" "); tft.print(LP100MEM,0); tft.print("  ");}
   
    if (odometrMEM<10) {tft.setCursor(69,34); tft.print("   "); tft.print(odometrMEM,0); tft.print("   "); }
    if (odometrMEM>=10 && odometrMEM<100) {tft.setCursor(69,34);tft.print("  "); tft.print(odometrMEM,0); tft.print("  ");}
    if (odometrMEM>=100 && odometrMEM<1000) {tft.setCursor(69,34); tft.print(" "); tft.print(odometrMEM,0); tft.print(" ");}
    if (odometrMEM>=1000) {tft.setCursor(71,34); tft.print(odometrMEM,0);}  
    
    if (benz_potrachenoMEM<10) {tft.setCursor(69,65); tft.print("  "); tft.print(benz_potrachenoMEM,1); tft.print("  "); }
    if (benz_potrachenoMEM>=10 && benz_potrachenoMEM<100) {tft.setCursor(69,65); tft.print(" "); tft.print(benz_potrachenoMEM,1); tft.print(" ");}
    if (benz_potrachenoMEM>=100 && benz_potrachenoMEM<1000) {tft.setCursor(69,65); tft.print(benz_potrachenoMEM,1);}
    if (benz_potrachenoMEM>=1000) {tft.setCursor(71,65); tft.print(benz_potrachenoMEM,0);}  
    
    if (benzraschetMEM<10) {tft.setCursor(69,96); tft.print("   "); tft.print(benzraschetMEM,0); tft.print("   "); }
    if (benzraschetMEM>=10 && benzraschetMEM<100) {tft.setCursor(69,96); tft.print("  "); tft.print(benzraschetMEM,0); tft.print("  ");}
    if (benzraschetMEM>=100 && benzraschetMEM<1000) {tft.setCursor(69,96); tft.print(" "); tft.print(benzraschetMEM,0); tft.print(" ");}
    if (benzraschetMEM>=1000 && benzraschetMEM<10000) {tft.setCursor(71,96); tft.print(benzraschetMEM,0);}
    if (benzraschetMEM>=10000) {tft.setCursor(65,96); tft.print(benzraschetMEM,0);}    
    }
 ////////////////////////////////////////////////////////////////////////////////////////////////////////////  
  break;

  //// сброс счетчиков общего расхода на 0
  case 101: 
  if(draw==1) {// отрисовка экрана
     bmpDraw("menu101.bmp", 6, 27);//отрисовка картинки на sd
     draw=0;} 
  if (millis() - previousMillis1 > 6000) {menu = 1; draw=1; tft.clearScreen();}
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////  
  break;

  case 2:
  ///////////////////////БЕНЗИН В БАКЕ///////////////////////////////////////////
  if (millis() - previousMillis1 < 1800) {
     if(draw==1) {// отрисовка заголовка
      bmpDraw("menu2_1.bmp", 0, 0);//отрисовка картинки на sd
      tft.setFont(&rus);
      tft.fillRect(0,41,128,45,DARK_GREEN);
      tft.setTextColor(BLACK);
      strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[8]))); // вывод из flash 
      tft.setCursor(18,54); tft.print(buffer); //tft.print("БЕНЗОБАК");
      tft.setTextColor(WHITE);
      tft.setCursor(17,53); tft.print(buffer); //tft.print("БЕНЗОБАК");
      bmpDraw("menu2_2.bmp", 0, 86);//отрисовка картинки на sd
     draw=2;}   
         }
    else {
     if(draw==2) {// очистка экрана
     tft.clearScreen();
     draw=1;}
     if(draw==1) {// отрисовка экрана
     tft.drawRect(0,0,128,64,DARK_GREEN);
     tft.fillRect(0,64,128,22,DARK_GREEN);
     tft.setFont(&rus);//выбор шрифта
     tft.setTextColor(BLACK);
     tft.setCursor(6,63);
     strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[9]))); // вывод из flash
     tft.print(buffer); //tft.print("в баке литров");
     tft.setTextColor(WHITE);
     tft.setCursor(5,62);
     strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[9]))); // вывод из flash
     tft.print(buffer); //tft.print("в баке литров");
     draw=3;} 
     
     if (fuel<5 && draw==4) {tft.fillRect(11,5,41,54,BLACK);  draw=5;}
     if (draw==4 && fuel != fuel_prev) {draw=3;}
     if (draw==3) {
     if (fuel>=15) {bmpDraw("menu2_benz_G.bmp", 11, 5);}
     if (fuel>=5 && fuel<15) {bmpDraw("menu2_benz_Y.bmp", 11, 5);}
     if (fuel<5) {bmpDraw("menu2_benz_R.bmp", 11, 5);}
     fuel_prev=fuel;
     draw=4;
     }
     if (fuel<5 && draw==5) {draw=3;}
     
     tft.setFont(&digit);//выбор шрифта
     if (fuel>=15) {tft.setTextColor(WHITE, BLACK);}
     if (fuel>=5 && fuel<15) {tft.setTextColor(YELLOW, BLACK);}
     if (fuel<5) {tft.setTextColor(RED, BLACK);}
     if (fuel>9.49) {tft.setCursor(64,5); tft.print(fuel,0);}
     else {tft.setCursor(64,5);  tft.print(" "); tft.print(fuel,0); tft.print(" ");}
     
     tft.setFont(&rus);//выбор шрифта
     tft.setTextColor(WHITE, BLACK);
     tft.setCursor(10,86);
     strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[10]))); // вывод из flash
     tft.print(buffer); //tft.print("остаток пути:");
     if (limitkm<10) { tft.setCursor(34,107); tft.print("   ");tft.print(int(limitkm)); tft.print("км  ");}
     if (limitkm>=10 && limitkm<100) { tft.setCursor(34,107); tft.print("  ");tft.print(int(limitkm)); tft.print("км  ");}
     if (limitkm>=100 && limitkm<1000) { tft.setCursor(34,107); tft.print(" ");tft.print(int(limitkm)); tft.print("км  ");}
     if (limitkm>=1000) { tft.setCursor(34,107); tft.print(int(limitkm)); tft.print("км  ");}
    }
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////  
  break;

  case 8:
   //////////////скорость////////////////////////////////////////////////////////////////////////////////////// 
  if (millis() - previousMillis1 < 1800) {
     if(draw==1) {// отрисовка заголовка
      bmpDraw("menu8_1.bmp", 0, 0);//отрисовка картинки на sd
      tft.setFont(&rus);
      tft.fillRect(0,41,128,45,VIOLET);
      tft.setTextColor(BLACK);
      strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[14]))); // вывод из flash 
      tft.setCursor(37,54);  tft.print(buffer); //tft.print("СКОРОСТЬ");
      tft.setTextColor(WHITE);
      tft.setCursor(36,53);  tft.print(buffer); //tft.print("СКОРОСТЬ");
      bmpDraw("menu8_2.bmp", 0, 86);//отрисовка картинки на sd
     draw=2;}   
         }
    else {
     if(draw==2) {// очистка экрана
     tft.clearScreen();
     draw=1;}
     if(draw==1) {// отрисовка экрана
     tft.fillRect(35,0,10,128,0x2945);
     tft.fillRect(83,0,10,128,0x2945);
     tft.fillRect(45,0,38,128,BLACK);
     tft.fillRect(61,5,6,16,WHITE);
     tft.fillRect(61,107,6,16,WHITE);
     tft.fillRect(19,26,91,75,0x2945);
     tft.drawRect(19,26,91,75,WHITE);
     tft.fillTriangle(72, 3, 78, 3, 75, 0, YELLOW);
     tft.fillTriangle(72, 124, 78, 124, 75, 127, YELLOW);
     tft.setFont(&rus);//выбор шрифта
     tft.setTextColor(YELLOW, 0x2945);
     tft.setCursor(48,76);
     tft.print("км/ч");
     draw=3;}
     if(D38_Read==LOW && draw==3) {
     tft.fillRect(35,0,10,128,GREEN);
     tft.fillRect(83,0,10,128,GREEN);
     tft.fillRect(45,0,38,128,BLACK);
     tft.fillRect(61,5,6,16,WHITE);
     tft.fillRect(61,107,6,16,WHITE);
     tft.fillRect(19,26,91,75,0x2800);
     tft.drawRect(19,26,91,75,WHITE);
     tft.fillTriangle(72, 3, 78, 3, 75, 0, YELLOW);
     tft.fillTriangle(72, 124, 78, 124, 75, 127, YELLOW);
     tft.setFont(&rus);//выбор шрифта
     tft.setTextColor(YELLOW, BLACK);
     tft.setCursor(69,3); tft.print("+");
     tft.setCursor(73,103); tft.print("-");
     tft.setFont(&rus);//выбор шрифта
     tft.setTextColor(GREEN, 0x2800);
     tft.setCursor(26,76);
     tft.print("авто км/ч");
     draw=4;}
     if(D38_Read==HIGH && draw==4) {
     tft.fillRect(35,0,10,128,0x2945);
     tft.fillRect(83,0,10,128,0x2945);
     tft.fillRect(45,0,38,128,BLACK);
     tft.fillRect(61,5,6,16,WHITE);
     tft.fillRect(61,107,6,16,WHITE);
     tft.fillRect(19,26,91,75,0x2945);
     tft.drawRect(19,26,91,75,WHITE);
     tft.fillTriangle(72, 3, 78, 3, 75, 0, YELLOW);
     tft.fillTriangle(72, 124, 78, 124, 75, 127, YELLOW);
     tft.setFont(&rus);//выбор шрифта
     tft.setTextColor(YELLOW, 0x2945);
     tft.setCursor(48,76);
     tft.print("км/ч");
     draw=3;
     }
     tft.setFont(&digit);//выбор шрифта
     if(D38_Read==HIGH) {tft.setTextColor(WHITE, 0x2945);} else {tft.setTextColor(WHITE, 0x2800);}
     if (speed_var<10) {tft.setCursor(20,28);  tft.print("   "); tft.print(speed_var); tft.print("  ");}
     if (speed_var>=10 && speed_var<100) {tft.setCursor(20,28);  tft.print("  "); tft.print(speed_var); tft.print(" ");}
     if (speed_var>=100) {tft.setCursor(20,28); tft.print(speed_var);}
     
     /////обороты///////
  if (rpm_var>=5000) {tft.fillRect(0,0,18,15,0xF800);}  else {tft.fillRect(0,0,18,15,BLACK);}  //5000
  if (rpm_var>=4000) {tft.fillRect(0,16,16,15,0xFA60);} else {tft.fillRect(0,16,16,15,BLACK);} //4000
  if (rpm_var>=3000) {tft.fillRect(0,32,14,15,0xFC40);} else {tft.fillRect(0,32,14,15,BLACK);} //3000
  if (rpm_var>=2500) {tft.fillRect(0,48,12,15,0xFE00);} else {tft.fillRect(0,48,12,15,BLACK);} //2500
  if (rpm_var>=2000) {tft.fillRect(0,64,10,15,0xDFE0);} else {tft.fillRect(0,64,10,15,BLACK);} //2000
  if (rpm_var>=1500) {tft.fillRect(0,80,8,15,0x97E0);}  else {tft.fillRect(0,80,8,15,BLACK);}  //1500
  if (rpm_var>=1000) {tft.fillRect(0,96,7,15,0x57E0);}  else {tft.fillRect(0,96,7,15,BLACK);}  //1000
  if (rpm_var>=650)  {tft.fillRect(0,112,5,16,0x17E0);} else {tft.fillRect(0,112,5,16,BLACK);} //650
  if (rpm_var>=650)  {
     tft.setFont(&rus_small); tft.setTextColor(WHITE, BLACK); tft.setCursor(7,114); tft.print("RPM");}
    else {tft.setFont(&rus_small); tft.setTextColor(BLACK, BLACK); tft.setCursor(7,114); tft.print("RPM");}
  /////нагрузка////////
  if (raschet_nagruzka_dvigatelya_var>=95) {tft.fillRect(110,0,18,15,0xF800);}  else {tft.fillRect(110,0,18,15,BLACK);}  //95%
  if (raschet_nagruzka_dvigatelya_var>=87) {tft.fillRect(112,16,16,15,0xFA60);} else {tft.fillRect(112,16,16,15,BLACK);} //87%
  if (raschet_nagruzka_dvigatelya_var>=75) {tft.fillRect(114,32,14,15,0xFC40);} else {tft.fillRect(114,32,14,15,BLACK);} //75%
  if (raschet_nagruzka_dvigatelya_var>=62) {tft.fillRect(116,48,12,15,0xFE00);} else {tft.fillRect(116,48,12,15,BLACK);} //62%
  if (raschet_nagruzka_dvigatelya_var>=50) {tft.fillRect(118,64,10,15,0xDFE0);} else {tft.fillRect(118,64,10,15,BLACK);} //50%
  if (raschet_nagruzka_dvigatelya_var>=37) {tft.fillRect(120,80,8,15,0x97E0);}  else {tft.fillRect(120,80,8,15,BLACK);}  //37%
  if (raschet_nagruzka_dvigatelya_var>=25) {tft.fillRect(121,96,7,15,0x57E0);}  else {tft.fillRect(121,96,7,15,BLACK);}  //25%
  if (raschet_nagruzka_dvigatelya_var>=12) {tft.fillRect(123,112,5,16,0x17E0);} else {tft.fillRect(123,112,5,16,BLACK);} //12%
  if (raschet_nagruzka_dvigatelya_var>=12) {
     tft.setFont(&rus_small); tft.setTextColor(WHITE, BLACK); tft.setCursor(94,114); tft.print("LOAD");}
    else {tft.setFont(&rus_small); tft.setTextColor(BLACK, BLACK); tft.setCursor(94,114); tft.print("LOAD");}
    }
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////  
  break;

   case 3:
  ////////////// ТЕМПЕРАТУРА ДВИГАТЕЛЯ, ТЕМПЕРАТУРА НА ВПУСКЕ, НАПРЯЖЕНИЕ//////////////////////////////////////////
   if (millis() - previousMillis1 < 1800) {
     if(draw==1) {// отрисовка заголовка
      bmpDraw("menu3_1.bmp", 0, 0);//отрисовка картинки на sd
      tft.setFont(&rus);
      tft.fillRect(0,41,128,45,0x4A49);
      tft.setTextColor(BLACK);
      strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[15]))); // вывод из flash 
      tft.setCursor(11,43); tft.print(buffer); //tft.print("температура,");
      strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[16]))); // вывод из flash 
      tft.setCursor(14,61); tft.print(buffer); //tft.print("напряжение");
      tft.setTextColor(WHITE);
      strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[15]))); // вывод из flash 
      tft.setCursor(10,42); tft.print(buffer); //tft.print("температура,");
      strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[16]))); // вывод из flash 
      tft.setCursor(13,60); tft.print(buffer); //tft.print("напряжение");
      bmpDraw("menu3_2.bmp", 0, 86);//отрисовка картинки на sd
     draw=2;}   
         }
    else {
     if(draw==2) {// очистка экрана
     tft.clearScreen();
     draw=1;}
     if(draw==1) {// отрисовка экрана
     tft.fillRect(7,6,117,38,0x4A49);
     tft.fillRect(7,47,117,38,0x4A49);
     tft.fillRect(7,88,117,38,0x4A49);
     tft.drawRect(6,5,116,37,WHITE);
     tft.drawRect(6,46,116,37,WHITE);
     tft.drawRect(6,87,116,37,WHITE);
     tft.fillRect(12,8,43,23,0x2800);
     tft.drawRect(12,8,43,23,WHITE);
     tft.fillRect(12,50,43,23,0x2800);
     tft.drawRect(12,50,43,23,WHITE);
     tft.fillRect(12,91,43,23,0x2800);
     tft.drawRect(12,91,43,23,WHITE);
     bmpDraw("menu3_3.bmp", 58, 6);//отрисовка картинки на sd
     bmpDraw("menu3_4.bmp", 58, 53);//отрисовка картинки на sd
     bmpDraw("menu3_5.bmp", 80, 93);//отрисовка картинки на sd
     draw=0;}
     tft.setFont(&rus);
     tft.setTextColor(WHITE, 0x2800);
     if (tmp_var>=0 && tmp_var<10) {tft.setCursor(20,9); tft.print("  "); tft.print(tmp_var); tft.print("  ");}
     if (tmp_var>=10) {tft.setCursor(20,9); tft.print(" "); tft.print(tmp_var); tft.print(" ");}
     if (tmp_var>-10 && tmp_var<0) {tft.setCursor(20,9); tft.print(" "); tft.print(tmp_var); tft.print("  ");}
     if (tmp_var<=-10) {tft.setCursor(20,9); tft.print(tmp_var); tft.print(" ");}

     if (intake_air_temp_var>=0 && intake_air_temp_var<10) {tft.setCursor(20,51); tft.print("  "); tft.print(intake_air_temp_var); tft.print("  ");}
     if (intake_air_temp_var>=10) {tft.setCursor(20,51); tft.print(" "); tft.print(intake_air_temp_var); tft.print(" ");}
     if (intake_air_temp_var>-10 && intake_air_temp_var<0) {tft.setCursor(20,51); tft.print(" "); tft.print(intake_air_temp_var); tft.print("  ");}
     if (intake_air_temp_var<=-10) {tft.setCursor(20,51); tft.print(intake_air_temp_var); tft.print(" ");}
 
     if (vbat>9.95) {tft.setCursor(15,92); tft.print(vbat,1);}
         else {tft.setCursor(15,92); tft.print(" "); tft.print(vbat,1); tft.print(" ");}
         
     tft.setTextColor(WHITE);
     tft.setCursor(57,95); tft.print("в.");
     tft.setFont(&rus_small);
     tft.setTextColor(WHITE);
     strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[17]))); // вывод из flash 
     tft.setCursor(14,28); tft.print(buffer); //tft.print("температ. двигателя");
     strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[18]))); // вывод из flash 
     tft.setCursor(16,70); tft.print(buffer); //tft.print("температура впуска");
     strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[19]))); // вывод из flash 
     tft.setCursor(25,111); tft.print(buffer); //tft.print("напряжение акб");
    }
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////  
  break;

  case 4:
  /////////////ТОПЛИВНЫЕ КОРРЕКЦИИ....////////////////////////////////////////////////////////////////
   if (millis() - previousMillis1 < 1800) {
     if(draw==1) {// отрисовка заголовка
      bmpDraw("menu4_1.bmp", 0, 0);//отрисовка картинки на sd
      tft.setFont(&rus);
      tft.fillRect(0,41,128,45,MAGENTA);
      tft.setTextColor(BLACK);
      strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[20]))); // вывод из flash 
      tft.setCursor(19,43); tft.print(buffer); //tft.print("топливные");
      strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[21]))); // вывод из flash 
      tft.setCursor(23,61); tft.print(buffer); //tft.print("коррекции");
      tft.setTextColor(WHITE);
      strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[20]))); // вывод из flash 
      tft.setCursor(18,42); tft.print(buffer); //tft.print("топливные");
      strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[21]))); // вывод из flash 
      tft.setCursor(22,60); tft.print(buffer); //tft.print("коррекции");
      bmpDraw("menu4_2.bmp", 0, 86);//отрисовка картинки на sd
     draw=2;}   
         }
    else {
     if(draw==2) {// очистка экрана
     tft.clearScreen();
     draw=1;}
    if(draw==1) {// отрисовка экрана  
     tft.setTextColor(WHITE);
     strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[22]))); // вывод из flash 
     tft.setCursor(6,-4); tft.print(buffer); //tft.print("долгосрочная");
     strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[23]))); // вывод из flash 
     tft.setCursor(3,107); tft.print(buffer); //tft.print("краткосрочная");
     bmpDraw("scale.bmp", 0, 40);//отрисовка картинки на sd
     bmpDraw("scale1.bmp", 0, 71);//отрисовка картинки на sd
     draw=0;}
     tft.setTextColor(WHITE, BLACK);
     tft.setCursor(40,17); tft.print("  "); tft.print(long_term_val); tft.print("    ");  // долгосрочная
     tft.setCursor(40,90); tft.print("  "); tft.print(short_term_val); tft.print("    "); // краткосрочная
     
     tft.fillTriangle(59+prev_long_term_val*3, 61, 69+prev_long_term_val*3, 61, 64+prev_long_term_val*3, 56, BLACK);   // затираем
     tft.fillTriangle(59+prev_short_term_val*3, 66, 69+prev_short_term_val*3, 66, 64+prev_short_term_val*3, 71, BLACK);// затираем
     
     tft.fillTriangle(59+long_term_val*3, 61, 69+long_term_val*3, 61, 64+long_term_val*3, 56, WHITE);   // долгосрочная
     tft.fillTriangle(59+short_term_val*3, 66, 69+short_term_val*3, 66, 64+short_term_val*3, 71, WHITE);// краткосрочная
     
     prev_long_term_val=long_term_val;
     prev_short_term_val=short_term_val;
    }
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////  
  break;

  
  case 5:
  /////////////// нагрузка, обороты, маф, loop, давление впуска, угол зажигания ///////////////////////////////////////////////////////
  if (millis() - previousMillis1 < 1800) {
     if(draw==1) {// отрисовка заголовка
      bmpDraw("menu5_1.bmp", 0, 0);//отрисовка картинки на sd
      tft.setFont(&rus);
      tft.fillRect(0,41,128,45,WHITE);
      tft.setTextColor(BLACK);
      strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[24]))); // вывод из flash 
      tft.setCursor(32,43); tft.print(buffer); //tft.print("ДРУГИЕ");
      strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[25]))); // вывод из flash 
      tft.setCursor(24,63); tft.print(buffer); //tft.print("ДАТЧИКИ");
      bmpDraw("menu5_2.bmp", 0, 86);//отрисовка картинки на sd
     draw=2;}   
         }
    else {
     if(draw==2) {// очистка экрана
     tft.clearScreen();
     draw=1;}
     if(draw==1) {
      tft.fillRect(0,0,128,22,WHITE);
      tft.setFont(&rus);
      tft.setTextColor(BLACK);
      strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[26]))); // вывод из flash 
      tft.setCursor(18,-1); tft.print(buffer); //tft.print("остальное:");
     draw=0;}
     tft.setFont(&rus_small);
     tft.setTextColor(WHITE, BLACK);
     strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[27]))); // вывод из flash 
     tft.setCursor(0,24);  tft.print(buffer); //tft.print("НАГРУЗКА:");
       tft.print(" "); tft.print(raschet_nagruzka_dvigatelya_var,0); tft.print("%"); tft.print("     ");
     strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[28]))); // вывод из flash 
     tft.setCursor(0,39); tft.print(buffer); //tft.print("ОБОРОТЫ ДВС:"); 
       tft.print(" "); tft.print(rpm_var); tft.print("   ");
     strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[29]))); // вывод из flash 
     tft.setCursor(0,54); tft.print(buffer); //tft.print("МАФ СЕНСОР:"); 
       tft.print(" "); tft.print(maf_var); tft.print("   ");
     strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[30]))); // вывод из flash 
     tft.setCursor(0,69); tft.print(buffer); //tft.print("ТОПЛИВНЫЙ ЦИКЛ:"); 
       tft.print(" "); tft.print(fss_val); tft.print("   ");
     strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[31]))); // вывод из flash 
     tft.setCursor(0,84); tft.print(buffer); //tft.print("ДАВЛЕНИЕ ВПУСКА:");
       tft.print(" "); tft.print(dvk_var); tft.print("   ");
     strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[32]))); // вывод из flash 
     tft.setCursor(0,99); tft.print(buffer); //tft.print("УГОЛ ЗАЖИГАНИЯ:"); 
       tft.print(" "); tft.print(uoz_var,0); tft.print("'"); tft.print("     ");
     strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[33]))); // вывод из flash 
     tft.setCursor(0,114); tft.print(buffer); // tft.print("ДРОССЕЛЬ:"); 
       tft.print(" "); tft.print(polozh_dross_zaslon,0); tft.print("%"); tft.print("      ");
    }
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////  
  break;

  case 6:
  ///////////////////////////////// ОШИБКИ ДВИГАТЕЛЯ///////////////////////////////////////////////////////////////////////
  if (millis() - previousMillis1 < 3000) {
     if(draw==1 && flag4==1) {// отрисовка заголовка
      if (kol_check_engine_error_val>0) {
      bmpDraw("check_engin.bmp", 14, 56);//отрисовка картинки на sd  
      }
      else {
      bmpDraw("engin.bmp", 14, 62);//отрисовка картинки на sd
      }
      tft.setFont(&rus);
      tft.fillRect(0,0,128,45,YELLOW);
      tft.fillRect(0,123,128,5,YELLOW);
      tft.setTextColor(BLACK);
      if (kol_check_engine_error_val==0) {
        strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[38]))); // вывод из flash
        tft.setCursor(3,13); tft.print(buffer); //tft.print("НЕТ ОШИБОК");
      } else {
      strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[34]))); // вывод из flash 
      tft.setCursor(25,3); tft.print(buffer); //tft.print("ОШИБКИ");
      strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[35]))); // вывод из flash 
      tft.setCursor(12,20); tft.print(buffer); //tft.print("ДВИГАТЕЛЯ");
      }
      tft.setFont(&digit); 
      tft.setTextColor(0xFDE0);
      tft.setCursor(92,56); tft.print(kol_check_engine_error_val);
      draw=2;
        }   
       }
    else {
     if(draw==1 || draw==2) {// отрисовка экрана  
      tft.clearScreen();
      tft.fillRect(0,0,128,30,RED);
      tft.setFont(&rus);
      tft.setTextColor(BLACK);
      strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[36]))); // вывод из flash 
      tft.setCursor(25,-4); tft.print(buffer); //tft.print("основные");
      strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[37]))); // вывод из flash 
      tft.setCursor(35,11); tft.print(buffer); //tft.print("ошибки");
      tft.setTextColor(WHITE);
      strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[36]))); // вывод из flash 
      tft.setCursor(24,-5); tft.print(buffer); //tft.print("основные");
      strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[37]))); // вывод из flash
      tft.setCursor(34,10); tft.print(buffer); //tft.print("ошибки");
      tft.fillTriangle(120, 124, 126, 124, 123, 127, YELLOW);
      tft.fillRect(0,114,128,2,RED);
     draw=0;}
      tft.setFont(&rus);
      tft.setTextColor(WHITE, BLACK);
      if (flag5){
      if (kod[0]=="0")
      { strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[38]))); // вывод из flash
        tft.setCursor(3,36); tft.print(buffer); //tft.print("НЕТ ОШИБОК");
      }
      else{
      strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[39]))); // вывод из flash  
      tft.setCursor(2,36); tft.print(buffer); //tft.print(">     "); 
      strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[40]))); // вывод из flash
      tft.print(kod[0]); tft.print(buffer); //tft.print("     <");
       if (kod[1]!="0")
        { strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[39]))); // вывод из flash  
          tft.setCursor(2,61); tft.print(buffer); //tft.print(">     "); 
          strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[40]))); // вывод из flash
          tft.print(kod[1]); tft.print(buffer); //tft.print("     <");
        }
       if (kod[2]!="0")
        { strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[39]))); // вывод из flash 
          tft.setCursor(2,86); tft.print(buffer); //tft.print(">     ");
          strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[40]))); // вывод из flash
          tft.print(kod[2]); tft.print(buffer); //tft.print("     <");
        }
      tft.setFont(&rus_small);
      tft.setTextColor(WHITE, BLACK);  
      strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[41]))); // вывод из flash
      tft.setCursor(0,116);  tft.print(buffer); //tft.print("      стереть: кн1+кн2      ");
      if (key3_2sec==1 && rpm_var>400) {tone(46, 800, 500);
        tft.setTextColor(RED, BLACK); 
        strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[42]))); // вывод из flash
        tft.setCursor(0,116); tft.print(buffer); //tft.print("выключить двигатель!!!");
        }
      }
     }
    }
  break;

  case 61:
  if(draw==1) {// отрисовка экрана  
      tft.clearScreen();
      tft.fillRect(0,0,128,30,GREEN);
      tft.setFont(&rus);
      tft.setTextColor(BLACK);
      strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[43]))); // вывод из flash
      tft.setCursor(17,-4); tft.print(buffer); //tft.print("результаты");
      strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[106]))); // вывод из flash
      tft.setCursor(39,11); tft.print(buffer); //tft.print("тестов");
      tft.setTextColor(WHITE);
      strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[43]))); // вывод из flash
      tft.setCursor(16,-5); tft.print(buffer); // tft.print("результаты");
      strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[106]))); // вывод из flash
      tft.setCursor(38,10); tft.print(buffer); //tft.print("тестов");
      tft.fillTriangle(120, 34, 126, 34, 123, 31, YELLOW);
      tft.fillRect(0,114,128,2,GREEN);
     draw=0;}
      tft.setFont(&rus);
      tft.setTextColor(WHITE, BLACK);
      if (flag5){
      if (kod[0]=="0")
      { strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[38]))); // вывод из flash
        tft.setCursor(3,36); tft.print(buffer); //tft.print("НЕТ ОШИБОК");
      }
      else{
      strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[39]))); // вывод из flash  
      tft.setCursor(2,36); tft.print(buffer); //tft.print(">     "); 
      strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[40]))); // вывод из flash
      tft.print(kod[0]); tft.print(buffer); //tft.print("     <");
       if (kod[1]!="0")
        { strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[39]))); // вывод из flash  
          tft.setCursor(2,61); tft.print(buffer); //tft.print(">     "); 
          strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[40]))); // вывод из flash
          tft.print(kod[1]); tft.print(buffer); //tft.print("     <");
        }
       if (kod[2]!="0")
        { strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[39]))); // вывод из flash 
          tft.setCursor(2,86); tft.print(buffer); //tft.print(">     ");
          strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[40]))); // вывод из flash
          tft.print(kod[2]); tft.print(buffer); //tft.print("     <");
        }
      tft.setFont(&rus_small);
      tft.setTextColor(WHITE, BLACK);  
      strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[41]))); // вывод из flash
      tft.setCursor(0,116);  tft.print(buffer); //tft.print("      стереть: кн1+кн2      ");
      if (key3_2sec==1 && rpm_var>400) {tone(46, 800, 500);
        tft.setTextColor(RED, BLACK); 
        strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[42]))); // вывод из flash
        tft.setCursor(0,116); tft.print(buffer); //tft.print("выключить двигатель!!!");
        }
      }
     }
  break;

  case 106:// сброс ошибок да, нет
  if(draw==1) {// отрисовка экрана
     bmpDraw("menu106.bmp", 6, 27);//отрисовка картинки на sd
     draw=0;} 
  if (millis() - previousMillis1 > 6000) {menu = 6; flag5=0; draw=1; tft.clearScreen();}
  break;

  case 107:// ок
  if(draw==1) {// отрисовка экрана
     bmpDraw("menu107.bmp", 6, 27);//отрисовка картинки на sd
     draw=0;} 
 if (millis() - previousMillis1 > 2000) {SearchErrorEngine()
 ; menu = 6; previousMillis1 = millis(); draw=1; tft.clearScreen();} 
  break;
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   ///////////////////////////////// НАСТРОЙКИ ///////////////////////////////////////////////////////////////////////
  case 7:
  if (millis() - previousMillis1 < 1800) {
     if(draw==1) {// отрисовка заголовка
      bmpDraw("menu7_1.bmp", 0, 0);//отрисовка картинки на sd
      tft.fillRect(0,41,128,45,CYAN);
      tft.setFont(&rus);
      tft.setTextColor(WHITE);
      strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[44]))); // вывод из flash
      tft.setCursor(10,54); tft.print(buffer); //tft.print("НАСТРОЙКИ");
      tft.setTextColor(BLACK);
      tft.setCursor(9,53); tft.print(buffer); //tft.print("НАСТРОЙКИ");
      bmpDraw("menu7_2.bmp", 0, 86);//отрисовка картинки на sd
     draw=2;}   
         }
    else {
     if(draw==2) {// очистка экрана
      tft.clearScreen();
      tft.fillRect(0,0,128,22,CYAN);
      tft.setFont(&rus);
      tft.setTextColor(BLACK);
      strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[45]))); // вывод из flash
      tft.setCursor(19,-1); tft.print(buffer); //tft.print("настройки:");
     draw=1;}
     if(draw==1) {// отрисовка экрана
     tft.setFont(&rus_small);
     strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[47]))); // вывод из flash
     tft.setCursor(2,29);  tft.setTextColor(YELLOW); tft.print(buffer); // tft.print("цена бензина: ");
       tft.setTextColor(WHITE, BLACK); tft.print(benzstoimost,1); tft.print("р."); tft.print("   ");
     strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[48]))); // вывод из flash
     tft.setCursor(2,44); tft.setTextColor(YELLOW); tft.print(buffer); // tft.print("сигнал скорости: "); 
       tft.setTextColor(WHITE, BLACK);
       if(speedlimit){
        strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[51]))); // вывод из flash
        tft.print(buffer); //tft.print("вкл   ");
        }
       if(!speedlimit){
        strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[52]))); // вывод из flash
        tft.print(buffer); //tft.print("выкл   ");
        }
     strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[49]))); // вывод из flash   
     tft.setCursor(2,59); tft.setTextColor(YELLOW); tft.print(buffer); //tft.print("корр. скорости: "); 
       tft.setTextColor(WHITE, BLACK); 
     if (speed_korrect_val==0.99) {
      strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[53]))); // вывод из flash 
      tft.print(buffer); //tft.print("235/75");
      }
     if (speed_korrect_val==1.02) {
      strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[54]))); // вывод из flash
      tft.print(buffer); //tft.print("245/75");
      }
     strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[55]))); // вывод из flash
     tft.setCursor(2,74); tft.setTextColor(YELLOW); tft.print(buffer); //tft.print("отсрочка рег: "); 
     tft.setTextColor(WHITE, BLACK);
     if(reg_on==0) {
      strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[56]))); // вывод из flash
      tft.print(buffer); //tft.print("всегда вкл   ");
      }
     if(reg_on==1) {
      strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[57]))); // вывод из flash
      tft.print(buffer); //tft.print("через час    ");
      }
     if(reg_on==2) {
      strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[58]))); // вывод из flash
      tft.print(buffer); //tft.print("3 часа       ");
      }
     if(reg_on==3) {
      strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[59]))); // вывод из flash
      tft.print(buffer); //tft.print("выкл         ");
      }
     strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[60]))); // вывод из flash
     tft.setCursor(2,89); tft.setTextColor(YELLOW); tft.print(buffer); //tft.print("калибровка бензобака");
    
     if(sel==0 && par==0) {tft.drawRect(0,28,128,14, BLACK); tft.drawFastHLine(0,41,128,WHITE);} 
      else {
        if(sel==0 && (par==1 || par==2)) {tft.drawRect(0,28,128,14,WHITE); par=2;} else {tft.drawRect(0,28,128,14, BLACK);}
      }
     if(sel==1 && par==0) {tft.drawRect(0,43,128,14,BLACK); tft.drawFastHLine(0,56,128,WHITE);}
      else {
        if(sel==1 && (par==1 || par==2)) {tft.drawRect(0,43,128,14,WHITE); par=2;} else {tft.drawRect(0,43,128,14,BLACK);} 
      }
     if(sel==2 && par==0) {tft.drawRect(0,58,128,14,BLACK); tft.drawFastHLine(0,71,128,WHITE);} 
      else {
        if(sel==2 && (par==1 || par==2)) {tft.drawRect(0,58,128,14,WHITE); par=2;} else {tft.drawRect(0,58,128,14,BLACK);} 
      }
     if(sel==3 && par==0) {tft.drawRect(0,73,128,14,BLACK); tft.drawFastHLine(0,86,128,WHITE);} 
      else {
        if(sel==3 && (par==1 || par==2)) {tft.drawRect(0,73,128,14,WHITE); par=2;} else {tft.drawRect(0,73,128,14,BLACK);} 
      }
     if(sel==4 && par==0) {tft.drawFastHLine(0,101,128,WHITE);} else {tft.drawFastHLine(0,101,128,BLACK);}
     if(sel==5 && par==0) {tft.drawFastHLine(0,115,128,WHITE);} else {tft.drawFastHLine(0,115,128,BLACK);}
     draw=0;}
      tft.setFont(&rus_small);
      tft.setTextColor(CYAN);
     if(millis()-time_inf>1500) {time_inf=millis(); inf++; tft.fillRect(0,117,128,11,BLACK);}
     if(inf>2) {inf=0;}
        
     if(inf==0){
      strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[46]))); // вывод из flash
      tft.setCursor(10,116); tft.print(buffer); // tft.print("выбор/сохр.: кн1+кн2");
      }
      
     if(inf==1){
     tft.setCursor(45,116);  tft.setTextColor(CYAN, BLACK);
      time_scr=millis()/1000;
     if (time_scr/60/60<10) { tft.print ("0"); }
     tft.print (time_scr/60/60);
     tft.print (":");
     if (time_scr/60%60<10) { tft.print ("0"); }
     tft.print ((time_scr/60)%60);
     tft.print (":");
     if (time_scr%60<10) { tft.print ("0"); }
     tft.print (time_scr%60);
     } 
     
     if(inf==2){
       tft.setCursor(42,116); tft.setTextColor(CYAN, BLACK); tft.print("RAM:"); tft.print(freeRam());
      }

   }
  break;

  case 108: /////////// Калибровка бака//////////////////////////////////////
  par=0;
  if(draw==1) {// отрисовка экрана
     tft.setFont(&rus_small);
     strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[61]))); // вывод из flash
     tft.setCursor(10,20); tft.setTextColor(YELLOW); tft.print(buffer); //tft.print("МАКС значение 100%"); 
     tft.setCursor(55,30);tft.setTextColor(WHITE, BLACK); tft.print(fuelmax);
     strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[62]))); // вывод из flash
     tft.setCursor(20,45); tft.setTextColor(YELLOW); tft.print(buffer); //tft.print("МИН значение 0%"); 
     tft.setCursor(55,55);tft.setTextColor(WHITE, BLACK); tft.print(fuelmin);
     tft.fillTriangle(80, 37, 86, 37, 83, 34, YELLOW);
     tft.fillTriangle(80, 59, 86, 59, 83, 62, YELLOW);
     strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[63]))); // вывод из flash
     tft.setCursor(7,90); tft.setTextColor(YELLOW); tft.print(buffer); //tft.print("текущее значение: "); 
     tft.setTextColor(WHITE, BLACK); tft.print(analog2,0); tft.print("  ");
      tft.setFont(&rus_small);
      tft.setTextColor(CYAN);  
      strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[64]))); // вывод из flash
      tft.setCursor(16,116);  tft.print(buffer); //tft.print("сохранить: кн1+кн2");
  draw=0;}
  break;

  
  case 110:
  if(draw==1) {// отрисовка экрана
    bmpDraw("80km.bmp", 9, 25);
    prev_menu_flag=1;
  draw=0;}
  if (millis() - previousMillis1 > 3000) {prev_menu_flag=0; menu = prev_menu; tft.clearScreen(); draw=1;}
  break;

  case 111:
  if(draw==1) {// отрисовка экрана
    bmpDraw("100km.bmp", 9, 25);
    prev_menu_flag=1;
  draw=0;}
  if (millis() - previousMillis1 > 3000) {prev_menu_flag=0; menu = prev_menu; tft.clearScreen(); draw=1;}
  break; 

  case 9:///////////////////////Лямбда1
  if (millis() - previousMillis1 < 1800) {
     if(draw==1) {// отрисовка заголовка
      bmpDraw("menu9_1.bmp", 0, 0);//отрисовка картинки на sd
      tft.fillRect(0,41,128,45,YELLOW);
      tft.setFont(&rus);
      tft.setTextColor(WHITE);
      tft.setCursor(29,54); tft.print("ЛЯМБДА");
      tft.setTextColor(BLACK);
      tft.setCursor(28,53); tft.print("ЛЯМБДА");
      bmpDraw("menu9_2.bmp", 0, 86);//отрисовка картинки на sd
     draw=2;}   
         }
    else {
  if(draw==2) {// очистка экрана
      tft.clearScreen();
      tft.setFont(&rus_small); 
      tft.setTextColor(WHITE);
      if (!time_graph_fag) {tft.setCursor(7,20); tft.print("1v/8s");
      tft.drawLine(1,54,3,54,WHITE);
      tft.drawLine(1,77,3,77,WHITE);
      tft.drawLine(1,100,3,100,WHITE);
      tft.drawLine(104,126,104,124,WHITE);
      tft.drawLine(90,126,90,124,WHITE);
      tft.drawLine(76,126,76,124,WHITE);
      tft.drawLine(62,126,62,124,WHITE);
      tft.drawLine(48,126,48,124,WHITE);
      tft.drawLine(34,126,34,124,WHITE);
      tft.drawLine(19,126,19,124,WHITE);}
      if (time_graph_fag) {tft.setCursor(7,20); tft.print("1v/4s");
      tft.drawLine(1,54,3,54,WHITE);
      tft.drawLine(1,77,3,77,WHITE);
      tft.drawLine(1,100,3,100,WHITE);
      tft.drawLine(90,126,90,124,WHITE);
      tft.drawLine(61,126,61,124,WHITE);
      tft.drawLine(31,126,31,124,WHITE);}
      tft.fillRect(0,0,128,22,YELLOW);
      tft.setFont(&rus);
      tft.setTextColor(BLACK);
      tft.setCursor(1,-1); tft.print("лямбда1:");// 4,123 - 0
      tft.drawLine(3,128,3,24,WHITE);
      tft.drawLine(0,124,125,124,WHITE);
      tft.drawLine(2,25,0,27,WHITE);
      tft.drawLine(4,25,6,27,WHITE);
      tft.drawLine(124,123,122,121,WHITE);
      tft.drawLine(124,125,122,127,WHITE);
      tft.drawLine(1,31,3,31,WHITE); // 4,31 max val 123-31 (92 point)
      tft.drawLine(118,126,118,124,WHITE); // 123,118 max time 4-118 (114 point)
      tft.setFont(&rus_small); 
      tft.setTextColor(WHITE);
      tft.setCursor(124,110); tft.print("t");
      tft.fillTriangle(0, 18, 6, 18, 3, 21, BLACK);
      tft.setTextColor(BLACK, YELLOW);
     draw=0;  
     point_x_prev=4;
     point_y_prev=123;
     time_graph=millis();
     }
   if (!time_graph_fag) {
    if((millis()-time_graph)>8000) {
    time_graph=millis(); 
    tft.fillRect(4,31,115,93,BLACK); 
    point_x_prev=4; point_y_prev=123;
    lyambd_val_max=0; lyambd_val_min=0;}
 timeGraph=millis()-time_graph;  
 point_x= map(timeGraph, 0, 8000, 4, 118);
 point_y= map(lyambd_val, 0, 100, 123, 31);
   }
   if (time_graph_fag) {
    if((millis()-time_graph)>4000) {
    time_graph=millis(); 
    tft.fillRect(4,31,115,93,BLACK); 
    point_x_prev=4; point_y_prev=123;
    lyambd_val_max=0; lyambd_val_min=0;}
 timeGraph=millis()-time_graph;  
 point_x= map(timeGraph, 0, 4000, 4, 118);
 point_y= map(lyambd_val, 0, 100, 123, 31);
   }
   
 tft.drawLine(point_x_prev,point_y_prev,point_x,point_y,YELLOW);
 point_x_prev=point_x;
 point_y_prev=point_y;
 if(lyambd_val_max<lyambd_val) {lyambd_val_max=lyambd_val;}
 if(lyambd_val_min>lyambd_val || lyambd_val_min==0) {lyambd_val_min=lyambd_val;}
 
 tft.setCursor(82,-1); tft.print("max:"); tft.print(double (lyambd_val_max)/100);
 tft.setCursor(82,10); tft.print("min: "); tft.print(double (lyambd_val_min)/100); 
 }
  break;

  case 91:
  if(draw==2) {// очистка экрана
      tft.clearScreen();
      tft.setFont(&rus_small); 
      tft.setTextColor(WHITE);
      if (!time_graph_fag) {tft.setCursor(7,20); tft.print("1v/8s");
      tft.drawLine(1,54,3,54,WHITE);
      tft.drawLine(1,77,3,77,WHITE);
      tft.drawLine(1,100,3,100,WHITE);
      tft.drawLine(104,126,104,124,WHITE);
      tft.drawLine(90,126,90,124,WHITE);
      tft.drawLine(76,126,76,124,WHITE);
      tft.drawLine(62,126,62,124,WHITE);
      tft.drawLine(48,126,48,124,WHITE);
      tft.drawLine(34,126,34,124,WHITE);
      tft.drawLine(19,126,19,124,WHITE);}
      if (time_graph_fag) {tft.setCursor(7,20); tft.print("1v/4s");
      tft.drawLine(1,54,3,54,WHITE);
      tft.drawLine(1,77,3,77,WHITE);
      tft.drawLine(1,100,3,100,WHITE);
      tft.drawLine(90,126,90,124,WHITE);
      tft.drawLine(61,126,61,124,WHITE);
      tft.drawLine(31,126,31,124,WHITE);}
      tft.fillRect(0,0,128,22,GREEN);
      tft.setFont(&rus);
      tft.setTextColor(BLACK);
      tft.setCursor(1,-1); tft.print("лямбда2:");// 4,123 - 0
      tft.drawLine(3,128,3,24,WHITE);
      tft.drawLine(0,124,125,124,WHITE);
      tft.drawLine(2,25,0,27,WHITE);
      tft.drawLine(4,25,6,27,WHITE);
      tft.drawLine(124,123,122,121,WHITE);
      tft.drawLine(124,125,122,127,WHITE);
      tft.drawLine(1,31,3,31,WHITE); // 4,31 max val 123-31 (92 point)
      tft.drawLine(118,126,118,124,WHITE); // 123,118 max time 4-118 (114 point)
      tft.setFont(&rus_small); 
      tft.setTextColor(WHITE);
      tft.setCursor(124,110); tft.print("t");
      tft.fillTriangle(0, 4, 6, 4, 3, 1, BLACK);
      tft.setTextColor(BLACK, GREEN);
     draw=0;  
     point_x_prev=4;
     point_y_prev=123;
     time_graph=millis();
     }

   if (!time_graph_fag) {
    if((millis()-time_graph)>8000) {
    time_graph=millis(); 
    tft.fillRect(4,31,115,93,BLACK); 
    point_x_prev=4; point_y_prev=123;
    lyambd_val_max=0; lyambd_val_min=0;}
 timeGraph=millis()-time_graph;  
 point_x= map(timeGraph, 0, 8000, 4, 118);
 point_y= map(lyambd_val, 0, 100, 123, 31);
   }
   if (time_graph_fag) {
    if((millis()-time_graph)>4000) {
    time_graph=millis(); 
    tft.fillRect(4,31,115,93,BLACK); 
    point_x_prev=4; point_y_prev=123;
    lyambd_val_max=0; lyambd_val_min=0;}
 timeGraph=millis()-time_graph;  
 point_x= map(timeGraph, 0, 4000, 4, 118);
 point_y= map(lyambd_val, 0, 100, 123, 31);
   }
 tft.drawLine(point_x_prev,point_y_prev,point_x,point_y,GREEN);
 point_x_prev=point_x;
 point_y_prev=point_y;
 if(lyambd_val_max<lyambd_val) {lyambd_val_max=lyambd_val;}
 if(lyambd_val_min>lyambd_val || lyambd_val_min==0) {lyambd_val_min=lyambd_val;}
 
 tft.setCursor(82,-1); tft.print("max:"); tft.print(double (lyambd_val_max)/100);
 tft.setCursor(82,10); tft.print("min: "); tft.print(double (lyambd_val_min)/100); 
  break;

 case 999: /////////// лицензия //////////////////////////////////////
  par=0;
  if(draw==1) {// отрисовка экрана
     tft.setFont(&rus_small); tft.setTextColor(YELLOW);
     tft.setCursor(15,0); tft.print("Лицензия freeware"); 
     tft.setTextColor(WHITE);
     tft.setCursor(0,20);
     tft.print("Идея, графическая оболочка, а так же тестирование и отладка программного кода Denis(produmann) github.com/produmann Протокол связи Ivor Wanders github.com/iwanders"); 
    
  draw=0;}
  break;
  
  }
 }
 else{
  if (draw!=101) {bmpDraw("timeout.bmp", 9, 22); draw=101;} 
      }
}
else{
   if(draw != 100) {// отрисовка заголовка
      tft.clearScreen();
      bmpDraw("off.bmp", 0, 0);//отрисовка картинки на sd
     draw=100;}   
 }
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////////////////////////////////
}
}





///////////////// ПОДПРОРАММЫ ДАТЧИКОВ///////////////////////

void Speed(void) {// скорость
  if (init_success){
    key(); // обработчик кнопок
    if(key1 || key2 || key3 || key1_sec || key3_2sec) {LCD();}  // немедленная отработка кнопок при нажатии
    res = obd.getPID(0x0D, 0x01, 1) ;
            if (res){init_log=0;
               speed_var = obd.readUint8()*speed_korrect_val;
            } else {init_log++;}
  }
}

void Tmp(void) {// температура
  if (init_success){
    key(); // обработчик кнопок
    if(key1 || key2 || key3 || key1_sec || key3_2sec) {LCD();}  // немедленная отработка кнопок при нажатии
    res = obd.getPID(0x05, 0x01, 1) ;
            if (res){init_log=0;
               tmp_var = obd.readUint8()-40;
            } else {init_log++;}
  }
}

void Rpm(void) {  // обороты
   if (init_success){
    key(); // обработчик кнопок
    if(key1 || key2 || key3 || key1_sec || key3_2sec) {LCD();}  // немедленная отработка кнопок при нажатии
    res = obd.getPID(0x0C, 0x01, 2) ;
            if (res){init_log=0;
               rpm_var = obd.readUint16()/4;
            } else {init_log++;}
  }
  if (rpm_var<399) { 
      rpm_var=0;
    }
} 

void IntakeAirTemp(void) {// Температура всасываемого воздуха
  if (init_success){
    key(); // обработчик кнопок
    if(key1 || key2 || key3 || key1_sec || key3_2sec) {LCD();}  // немедленная отработка кнопок при нажатии
    res = obd.getPID(0x0F, 0x01, 1) ;
            if (res){init_log=0;
               intake_air_temp_var = obd.readUint8()-40;
            } else {init_log++;}
  }
}

void Rnd(void) {// Расчетное значение нагрузки на двигатель
  if (init_success){
    key(); // обработчик кнопок
    if(key1 || key2 || key3 || key1_sec || key3_2sec) {LCD();}  // немедленная отработка кнопок при нажатии
    res = obd.getPID(0x04, 0x01, 1) ;
            if (res){init_log=0;
               raschet_nagruzka_dvigatelya_var = obd.readUint8()*100/255;
            } else {init_log++;}
  }
}

void Maf(void) {// Массовый расход воздуха
  if (init_success){
    key(); // обработчик кнопок
    if(key1 || key2 || key3 || key1_sec || key3_2sec) {LCD();}  // немедленная отработка кнопок при нажатии
    res = obd.getPID(0x10, 0x01, 2) ;
            if (res){init_log=0;
               maf_var = double(obd.readUint16())/100;
            } else {init_log++;}
  }
}

void ShortTerm(void) {//   Short term fuel % trim—Bank 1 06
  if (init_success){
    key(); // обработчик кнопок
    if(key1 || key2 || key3 || key1_sec || key3_2sec) {LCD();}  // немедленная отработка кнопок при нажатии
    res = obd.getPID(0x06, 0x01, 1) ;
            if (res){init_log=0;
               short_term_val = double((obd.readUint8()-128)* 100)/128;
            } else {init_log++;}
  }
}

void LongTerm(void) { //   Long term fuel % trim—Bank 1 07
  if (init_success){
    key(); // обработчик кнопок
    if(key1 || key2 || key3 || key1_sec || key3_2sec) {LCD();}  // немедленная отработка кнопок при нажатии
    res = obd.getPID(0x07, 0x01, 1) ;
            if (res){init_log=0;
               long_term_val = double((obd.readUint8()-128)* 100)/128;
            } else {init_log++;}
  }
}

void SearchErrorEngine(void) { //   Проверка ошибок двигателя 01
   if (init_success){
    key(); // обработчик кнопок
    if(key1 || key2 || key3 || key1_sec || key3_2sec) {LCD();}  // немедленная отработка кнопок при нажатии
    res = obd.getPID(0x01, 0x01, 4) ;
            if (res){init_log=0;
               kol_check_engine_error_val = String(obd.readUint8(), HEX).toInt();
               if (kol_check_engine_error_val>0) {kol_check_engine_error_val=kol_check_engine_error_val-80;}
            } else {init_log++;}
  }
 if (kol_check_engine_error_val>0)
    {error03(); 
    tone(46, 4700, 1000);
    menu_scr=0; menu = 6; previousMillis1 = millis();}
  flag4=1;
}

void error03(void) {     //    Error codes view основные
   kod[0]="0"; kod[1]="0"; kod[2]="0";    
 if (init_success){
 res = obd.readTroubleCodes();
  if (res){init_log=0; flag5=1;
    for (int i=0; i<3; i++){
      uint16_t trouble_code = obd.getTroubleCode(i);
             if (trouble_code != 0)
                  {OBD9141::decodeDTC(trouble_code, dtc_buf); kod[i]=dtc_buf;}
               }
   } else {init_log++;}
    }
}

void error07(void) { //    Запрос результатов диагностики непрерывно действующих тестов
 kod[0]="0"; kod[1]="0"; kod[2]="0";     
 if (init_success){
 res = obd.readPendingTroubleCodes();
  if (res){init_log=0; flag5=1;
    for (int i=0; i<3; i++){
      uint16_t trouble_code = obd.getTroubleCode(i);
             if (trouble_code != 0)
                  {OBD9141::decodeDTC(trouble_code, dtc_buf); kod[i]=dtc_buf;}
               }
   } else {init_log++;}
    }
}

void clearError(void) { //   очистка ошибок 04
  if (init_success){
  clERRok = obd.clearTroubleCodes();
   }
}

void FuelSystemStatus(void) {   //   Проверка, если форсунки отключены то 1, если работают то 2   03
 if (init_success){
    key(); // обработчик кнопок
    if(key1 || key2 || key3 || key1_sec || key3_2sec) {LCD();}  // немедленная отработка кнопок при нажатии
    res = obd.getPID(0x03, 0x01, 2) ;
            if (res){init_log=0;
               fss_val = obd.readUint8();
            } else {init_log++;}
  }
}

void DavlenVpuskKoll(void) {// давление впускного коллектора 0B
   if (init_success){
    key(); // обработчик кнопок
    if(key1 || key2 || key3 || key1_sec || key3_2sec) {LCD();}  // немедленная отработка кнопок при нажатии
    res = obd.getPID(0x0B, 0x01, 1) ;
            if (res){init_log=0;
               dvk_var = obd.readUint8();
            } else {init_log++;}
  }
}

void Uoz(void) {  // Угол опережения зажигания  0E
 if (init_success){
    key(); // обработчик кнопок
    if(key1 || key2 || key3 || key1_sec || key3_2sec) {LCD();}  // немедленная отработка кнопок при нажатии
    res = obd.getPID(0x0E, 0x01, 1) ;
            if (res){init_log=0;
               uoz_var = obd.readUint8()/2-64;
            } else {init_log++;}
  }
}

void Pdz(void) {// Положение дроссельной заслонки  
    if (init_success){
    key(); // обработчик кнопок
    if(key1 || key2 || key3 || key1_sec || key3_2sec) {LCD();}  // немедленная отработка кнопок при нажатии
    res = obd.getPID(0x11, 0x01, 1) ;
            if (res){init_log=0;
               polozh_dross_zaslon = obd.readUint8()*100/255;
            } else {init_log++;}
  } 
}

void Oxygen1(void) {  // Лямбда1
   if (init_success){
    key(); // обработчик кнопок
    if(key1 || key2 || key3 || key1_sec || key3_2sec) {LCD();}  // немедленная отработка кнопок при нажатии
    res = obd.getPID(0x14, 0x01, 2) ;
          if (res){init_log=0;
           lyambd_val = obd.readUint8()/2;
           } else {init_log++;}
  }
}

void Oxygen2(void) {  // Лямбда2
   if (init_success){
    key(); // обработчик кнопок
    if(key1 || key2 || key3 || key1_sec || key3_2sec) {LCD();}  // немедленная отработка кнопок при нажатии
    res = obd.getPID(0x15, 0x01, 2) ;
            if (res){init_log=0;
               lyambd_val = obd.readUint8()/2;
             } else {init_log++;}
  }
}

void fuel_var(void){
//////////////////////// топливо в баке////////////////////////////
 if (millis() - previousMillis2 > 500)
 {if (ii <= 99){
      analog[ii] = analogRead(0);
      ii++;}
  previousMillis2 = millis();     
 }
 if (ii==100)
 {
  ii=0;
  analog1=0;
  while (ii<=99){
      analog1 = analog1+analog[ii];
      ii++;}
  ii=0;
      analog2 = analog1/100; // среднее значение датчика топлива
 }
 fvar=((analog2-fuelmin)*1000/(fuelmin-fuelmax))/(-10); // выражаем в %%% количество топлива
 if (fvar>100) {fvar=100;}
 if (fvar<0) {fvar=0;}
 fuel=bak*fvar/100; // количество литров в баке
}

/////////////////////////////////ДОПОЛНИТЕЛЬНЫЕ ПРОГРАММЫ/////////////////////////////////////////////////////////////////////
void SDwrite(void){
  if(!edit){
    if (!SD.begin(__SDCS,SPI_FULL_SPEED)) {
    tft.fillRect(0,41,128,45,RED);
    tft.setFont(&rus); tft.setTextColor(WHITE); 
    strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[65]))); // вывод из flash текста
    tft.setCursor(25,42); tft.print(buffer); //tft.print("ОШИБКА");
    strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[66]))); // вывод из flash текста
    tft.setCursor(2,62); tft.print(buffer); //tft.print("ВСТАВЬТЕ SD");
    tone(46, 800, 200); delay(500);
  }
   else{ 
    SD.remove("benz");// удалить старый файл
    myFile = SD.open("benz", FILE_WRITE);// создать\открыть файл
    if (myFile) {
            myFile.println(benz_potrachenoMEM*1000, 0);
        myFile.close();
    } else {
        tft.fillRect(0,41,128,45,RED); tft.setFont(&rus); tft.setTextColor(WHITE, RED); tft.setCursor(2,53); strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[50]))); tft.print(buffer);/*tft.print("ОШИБКА ->SD");*/ tone(46, 800, 200); delay(500);
    }
    SD.remove("odometr");// удалить старый файл
    myFile = SD.open("odometr", FILE_WRITE);// создать\открыть файл
    if (myFile) {
            myFile.println(odometrMEM*1000, 0);
        myFile.close();
    } else {
        tft.fillRect(0,41,128,45,RED); tft.setFont(&rus); tft.setTextColor(WHITE, RED); tft.setCursor(2,53); strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[50]))); tft.print(buffer);/*tft.print("ОШИБКА ->SD");*/ tone(46, 800, 200); delay(500);
    }
    SD.remove("benzst");// удалить старый файл
    myFile = SD.open("benzst", FILE_WRITE);// создать\открыть файл
    if (myFile) {
            myFile.println(benzstoimost*100, 0);
        myFile.close();
    } else {
        tft.fillRect(0,41,128,45,RED); tft.setFont(&rus); tft.setTextColor(WHITE, RED); tft.setCursor(2,53); strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[50]))); tft.print(buffer);/*tft.print("ОШИБКА ->SD");*/ tone(46, 800, 200); delay(500);
    }
    SD.remove("spdlim");// удалить старый файл
    myFile = SD.open("spdlim", FILE_WRITE);// создать\открыть файл
    if (myFile) {
            myFile.println(speedlimit);
        myFile.close();
    } else {
       tft.fillRect(0,41,128,45,RED); tft.setFont(&rus); tft.setTextColor(WHITE, RED); tft.setCursor(2,53); strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[50]))); tft.print(buffer);/*tft.print("ОШИБКА ->SD");*/ tone(46, 800, 200); delay(500);
    }
    SD.remove("fuelmin");// удалить старый файл
    myFile = SD.open("fuelmin", FILE_WRITE);// создать\открыть файл
    if (myFile) {
            myFile.println(fuelmin);
        myFile.close();
    } else {
        tft.fillRect(0,41,128,45,RED); tft.setFont(&rus); tft.setTextColor(WHITE, RED); tft.setCursor(2,53); strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[50]))); tft.print(buffer);/*tft.print("ОШИБКА ->SD");*/ tone(46, 800, 200); delay(500);
    }
    SD.remove("fuelmax");// удалить старый файл
    myFile = SD.open("fuelmax", FILE_WRITE);// создать\открыть файл
    if (myFile) {
            myFile.println(fuelmax);
        myFile.close();
    } else {
        tft.fillRect(0,41,128,45,RED); tft.setFont(&rus); tft.setTextColor(WHITE, RED); tft.setCursor(2,53); strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[50]))); tft.print(buffer);/*tft.print("ОШИБКА ->SD");*/ tone(46, 800, 200); delay(500);
    }
    SD.remove("spdkor");// удалить старый файл
    myFile = SD.open("spdkor", FILE_WRITE);// создать\открыть файл
    if (myFile) {
            myFile.println(speed_korrect_val*100, 0);
        myFile.close();
    } else {
        tft.fillRect(0,41,128,45,RED); tft.setFont(&rus); tft.setTextColor(WHITE, RED); tft.setCursor(2,53); strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[50]))); tft.print(buffer);/*tft.print("ОШИБКА ->SD");*/ tone(46, 800, 200); delay(500);
    }
   }
  }
}

void SDread(void){
  if(!edit){
    if (!SD.begin(__SDCS,SPI_FULL_SPEED)) {
    tft.fillRect(0,41,128,45,RED);
    tft.setFont(&rus); tft.setTextColor(WHITE); 
    strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[65]))); // вывод из flash текста
    tft.setCursor(25,42); tft.print(buffer); //tft.print("ОШИБКА");
    strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[66]))); // вывод из flash текста
    tft.setCursor(2,62); tft.print(buffer); //tft.print("ВСТАВЬТЕ SD");
    tone(46, 800, 200); delay(500);
  }
   else {
  myFile = SD.open("benz");// открываем файл
  var="";
    if (myFile) {
   while (myFile.available()) {
     var=var+char(myFile.read());   
    }
    myFile.close();// закрыть файл
    benz_potrachenoMEM=double(var.toInt())/1000;
    } else {
      tft.fillRect(0,41,128,45,RED); tft.setFont(&rus); tft.setTextColor(WHITE, RED); tft.setCursor(2,53); strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[67]))); tft.print(buffer); /*tft.print("ОШИБКА SD->");*/ tone(46, 800, 200); delay(500);
    }
    myFile = SD.open("odometr");// открываем файл
  var="";
    if (myFile) {
   while (myFile.available()) {
     var=var+char(myFile.read());   
    }
    myFile.close();// закрыть файл
    odometrMEM=double(var.toInt())/1000;
    } else {
         tft.fillRect(0,41,128,45,RED); tft.setFont(&rus); tft.setTextColor(WHITE, RED); tft.setCursor(2,53); strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[67]))); tft.print(buffer); /*tft.print("ОШИБКА SD->");*/ tone(46, 800, 200); delay(500);
    }
    myFile = SD.open("benzst");// открываем файл
  var="";
    if (myFile) {
   while (myFile.available()) {
     var=var+char(myFile.read());   
    }
    myFile.close();// закрыть файл
    if(double(var.toInt())>0){
    benzstoimost=double(var.toInt())/100;}
    
    } else {
        tft.fillRect(0,41,128,45,RED); tft.setFont(&rus); tft.setTextColor(WHITE, RED); tft.setCursor(2,53); strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[67]))); tft.print(buffer); /*tft.print("ОШИБКА SD->");*/ tone(46, 800, 200); delay(500);
    }
     myFile = SD.open("spdlim");// открываем файл
  var="";
    if (myFile) {
   while (myFile.available()) {
     var=var+char(myFile.read());   
    }
    myFile.close();// закрыть файл
    speedlimit=var.toInt();
    } else {
        tft.fillRect(0,41,128,45,RED); tft.setFont(&rus); tft.setTextColor(WHITE, RED); tft.setCursor(2,53); strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[67]))); tft.print(buffer); /*tft.print("ОШИБКА SD->");*/ tone(46, 800, 200); delay(500);
    }
     myFile = SD.open("fuelmin");// открываем файл
  var="";
    if (myFile) {
   while (myFile.available()) {
     var=var+char(myFile.read());   
    }
    myFile.close();// закрыть файл
    if(!((var.toInt())==0)) {fuelmin=var.toInt();}
    } else {
        tft.fillRect(0,41,128,45,RED); tft.setFont(&rus); tft.setTextColor(WHITE, RED); tft.setCursor(2,53); strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[67]))); tft.print(buffer); /*tft.print("ОШИБКА SD->");*/ tone(46, 800, 200); delay(500);
    }
     myFile = SD.open("fuelmax");// открываем файл
  var="";
    if (myFile) {
   while (myFile.available()) {
     var=var+char(myFile.read());   
    }
    myFile.close();// закрыть файл
    if(!((var.toInt())==0)) {fuelmax=var.toInt();}
    } else {
        tft.fillRect(0,41,128,45,RED); tft.setFont(&rus); tft.setTextColor(WHITE, RED); tft.setCursor(2,53); strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[67]))); tft.print(buffer); /*tft.print("ОШИБКА SD->");*/ tone(46, 800, 200); delay(500);
    }
    myFile = SD.open("spdkor");// открываем файл
  var="";
    if (myFile) {
   while (myFile.available()) {
     var=var+char(myFile.read());   
    }
    myFile.close();// закрыть файл
    speed_korrect_val=double(var.toInt())/100;
    } else {
         tft.fillRect(0,41,128,45,RED); tft.setFont(&rus); tft.setTextColor(WHITE, RED); tft.setCursor(2,53); strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[67]))); tft.print(buffer); /*tft.print("ОШИБКА SD->");*/ tone(46, 800, 200); delay(500);
    }
   }
 }
}


void key(void){
  ///////////////////// обработчик кнопок ////////////////////////////////////////
  key1=0; key2=0; key3=0;
  key1_sec=0; key3_2sec=0;
  
 if (!(D2_Read) && !(D3_Read) && flag2==0)// нажатие 2х кнопок flag2
  { if(comp&&!alarm){tone(46, 4500, 200);}// звуки только при включенном БК
    key3_alarm=1; // для отключения сигналки
    flag2=1; flag=0; flag1=0; previousMillis3 = millis();}
 if (((D2_Read) && (D3_Read)) && flag2==1)
  {flag2=0; key3=1;}
 
  if(!(D2_Read) && flag==0 && flag1==0 && flag2==0) // кнопка 1 flag
 {if(comp && !alarm && !(D38_Read==LOW && !menu_scr && menu==8)){tone(46, 4500, 50);}// звуки только при включенном БК и выкл круиз
  flag=1; previousMillis3 = millis();}    
 if((D2_Read) && flag==1) {flag=0; key1=1;}

 if(!(D3_Read) && flag1==0 && flag==0 && flag2==0) // кнопка 2 flag1
 {if(comp && !alarm && !(D38_Read==LOW && !menu_scr && menu==8)){tone(46, 4500, 50);}// звуки только при включенном БК и выкл круиз
  flag1=1; previousMillis3 = millis();}
 if((D3_Read) && flag1==1) {flag1=0; key2=1;}
 
///////////// комбинации /////////////////////////////////////////////////////////
if(comp && !alarm && !(D38_Read==LOW && !menu_scr && menu==8)){// комбинации только при включенном БК и выкл круиз
if(flag && (millis() - previousMillis3 > 1000) && !flag3) {tone(46, 4500, 100); delay (120); tone(46, 4500, 100); flag3=1;} // оповещение кнопки 1 1сек
if(key1 && (millis() - previousMillis3 > 1000)) {key1_sec=1; key1=0; flag3=0;}                    // нажатие кнопки 1 1сек

if(flag2 && (millis() - previousMillis3 > 2000) && !flag3) {tone(46, 4500, 100); delay (120); tone(46, 4500, 100); flag3=1;} // оповещение 2х кнопок 2сек
if(key3 && (millis() - previousMillis3 > 2000)) {key3_2sec=1; key3=0; flag3=0;}                    // нажатие 2х кнопок 2сек
 }  
 //// КНОПКИ КРУИЗА
if(D38_Read==HIGH && !menu_scr && menu==8){
if(key1) {D40_High; cruise=1;}; //accel
if(key2) {D42_High; cruise=1;}; //decel
} 
if(D38_Read==LOW && !menu_scr && menu==8) {
    if(flag==1 && !cruise1) {D40_High; cruise1=1;}  //++  
    if(flag==0 && cruise1) {D40_Low; cruise1=0;}
    if(flag1==1 && !cruise2){D42_High; cruise2=1;}  //--
    if(flag1==0 && cruise2) {D42_Low; cruise2=0;}
    if(flag2==1) {D40_Low; D42_Low;}
}
if((millis() - previousMillis3 > 300) && cruise==1) {D40_Low; D42_Low; cruise=0;};// отжатие кнопки круиз
}
///////////////отрисовка картинки/////////////////////////////
void bmpDraw(const char *filename, uint8_t x, uint16_t y) {
if (!SD.begin(__SDCS,SPI_FULL_SPEED)) {
    tft.fillRect(0,41,128,45,RED);
    tft.setFont(&rus); tft.setTextColor(WHITE); 
    strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[65]))); // вывод из flash текста
    tft.setCursor(25,42); tft.print(buffer); //tft.print("ОШИБКА");
    strcpy_P(buffer, (char*)pgm_read_word(&(textMEM[66]))); // вывод из flash текста
    tft.setCursor(2,62); tft.print(buffer); //tft.print("ВСТАВЬТЕ SD");
    tone(46, 800, 200); delay(500);
    SDInited = false;
  }
  
  if (SDInited){

    File     bmpFile;
    uint16_t bmpWidth, bmpHeight;   // W+H in pixels
    uint8_t  bmpDepth;              // Bit depth (currently must be 24)
    uint32_t bmpImageoffset;        // Start of image data in file
    uint32_t rowSize;               // Not always = bmpWidth; may have padding
    uint8_t  sdbufferLen = BUFFPIXEL * 3;
    uint8_t  sdbuffer[sdbufferLen]; // pixel buffer (R+G+B per pixel)
    uint8_t  buffidx = sdbufferLen; // Current position in sdbuffer
    boolean  goodBmp = false;       // Set to true on valid header parse
    boolean  flip    = true;        // BMP is stored bottom-to-top
    uint16_t w, h, row, col;
    uint8_t  r, g, b;
    uint32_t pos = 0;

    if((x >= tft.width()) || (y >= tft.height())) return;

    // Open requested file on SD card
    if ((bmpFile = SD.open(filename)) == NULL) {
      tft.setCursor(0,0);
      tft.print("file not found!");
      return;
    }

    // Parse BMP header
    if(read16(bmpFile) == 0x4D42) { // BMP signature
      read32(bmpFile);
      (void)read32(bmpFile); // Read & ignore creator bytes
      bmpImageoffset = read32(bmpFile); // Start of image data
      // Read DIB header
      read32(bmpFile);
      bmpWidth  = read32(bmpFile);
      bmpHeight = read32(bmpFile);
      if(read16(bmpFile) == 1) { // # planes -- must be '1'
        bmpDepth = read16(bmpFile); // bits per pixel
        if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed
          goodBmp = true; // Supported BMP format -- proceed!
          rowSize = (bmpWidth * 3 + 3) & ~3;// BMP rows are padded (if needed) to 4-byte boundary
          if (bmpHeight < 0) {
            bmpHeight = -bmpHeight;
            flip      = false;
          }
          // Crop area to be loaded
          w = bmpWidth;
          h = bmpHeight;
          if((x+w-1) >= tft.width())  w = tft.width()  - x;
          if((y+h-1) >= tft.height()) h = tft.height() - y;
          tft.setAddrWindow(x, y, x+w-1, y+h-1);
          for (row=0; row<h; row++) { // For each scanline...
            if (flip){ // Bitmap is stored bottom-to-top order (normal BMP)
              pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
            } 
            else {     // Bitmap is stored top-to-bottom
              pos = bmpImageoffset + row * rowSize;
            }
            if (bmpFile.position() != pos) { // Need seek?
              bmpFile.seek(pos);
              buffidx = sdbufferLen; // Force buffer reload
            }
            for (col=0; col<w; col++) { // For each pixel...
              // Time to read more pixel data?
              if (buffidx >= sdbufferLen) { // Indeed
                bmpFile.read(sdbuffer, sdbufferLen);
                buffidx = 0; // Set index to beginning
              }
              // Convert pixel from BMP to TFT format, push to display
              b = sdbuffer[buffidx++];
              g = sdbuffer[buffidx++];
              r = sdbuffer[buffidx++];
            tft.pushColor(tft.Color565(r,g,b));
            } // end pixel
          } // end scanline
        } // end goodBmp
      }
    }

    bmpFile.close();
    if(!goodBmp) {
      tft.setCursor(0,0);
      tft.print("sd file bad!");
    }
  }
}


uint16_t read16(File &f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(File &f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}


int freeRam () { // показывает свободное место в ОЗУ
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}
