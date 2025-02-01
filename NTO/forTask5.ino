/*
EEG-8
Скетч оцифровывает сигналы c восьми сенсоров электроэнцефалограммы (ЭЭГ, EEG), устройства синхронизации, а также формирует тестовый сигнал "пила".  
Справочник по командам языка Arduino: http://arduino.ru/Reference 
*/

#include <TimerOne.h> // подключаем библиотеку TimerOne для задействования функций Таймера1 
/* предварительно данную библиотеку надо установить, для чего скачиваем ее 
на странице https://bitronicslab.com/neuromodelist, распаковываем архив и помещаем папку TimerOne
внутрь папки libraries, находящейся тут: "Мои документы/Arduino/libraries" 
Подробнее о TimerOne см. тут: http://robocraft.ru/blog/arduino/614.html */

byte i = 0;
int val;

// функция sendData вызывается каждый раз, когда срабатывает прерывание Таймера1 (проходит заданное число микросекунд)
void sendData() {
  int values[10];

  val = analogRead(A0);                   
  values[0] = val;
                                                     
  val = analogRead(A1);                  
  values[1] = val;

  val = analogRead(A2);                     
  values[2] = val;

  val = analogRead(A3);                     
  values[3] = val;

  val = analogRead(A4);                     
  values[4] = val;

  val = analogRead(A5);                     
  values[5] = val;

  val = analogRead(A6);                     
  values[6] = val;

  val = analogRead(A7);                     
  values[7] = val;

  val = analogRead(A8);                     
  values[8] = val; 

  values[9] = i; 
  

  for (int i = 0; i < 10; i++) {
    if(i == 9)
      Serial.print(values[9]);
    else
      Serial.print(map(values[i], 0, 1023, 0, 255)); // Отправка текущего значения массива
    if (i < 9) { // Для всех элементов, кроме последнего, добавляем запятую
      Serial.print(",");
    }
  }  

  Serial.println();
  i++;                                         
}


// функция setup вызывается однократно при запуске Arduino
void setup() {
  pinMode(13, OUTPUT);                     // Конфигурируем вывод 13 как выход (к нему подключет светодиод)
  digitalWrite(13, HIGH);                  // Устанавливаем на ней высокий логический уровень (включаем светодиод)
  Serial.begin(115200);                    // инициализируем Serial-порт на скорости 115200 Кбит/c. 
                                           // такую же скорость надо установить в программе для визуализации
  Timer1.initialize(3000);                 // инициализируем Таймер1, аргументом указываем интервал срабатывания - 3000 микросекунд 
                                           // (1 000 000 микросекунд = 1 сек)
  Timer1.attachInterrupt(sendData);        // как только проходит 3000 микросекунд - наступает прерывание (вызывается функция sendData)
}

void loop() {

}
