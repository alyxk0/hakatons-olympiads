#include <Servo.h> //добавляем библиотеку для работы сервоприводов

#define ZUMM 9 //обозначаем пин для зуммера
#define GREEN 0 //обозначаем пин для зеленого светодиода(загорается при напряжении левой руки)
#define YELLOW 1 //обозначаем пин для желтого светодиода(загорается при напряжении правой руки)
#define RED 12 //обозначаем пин для красного светодиода(загорается при напряжении обех рук)
#define PIN_TRIG 2 //обозначаем пин для УЗ датчика 
#define PIN_ECHO 3 //обозначаем пин для УЗ датчика
#define LEFT_BUTT A4
#define RIGHT_BUTT A5
#define RED_PODSTR 10
#define YELLOW_PODSTR 11
#define GREEN_PODSTR 7
//создаем обьекты для сервоприводов и называем их соответственно пальцам на руке
Servo bigfingSgib; //серво, отвечающая за сгиб большого пальца
Servo bigfingRotat; //серво, отвечающая за поворот большого пальца
Servo ukazfing; //серво, отвечающая за сгиб указательного пальца
Servo littlefing; //серво, отвечающая за сгиб мизинца
Servo fing2; //серво, отвечающая за сгиб двух пальцев(средний и безымянный)

//переменные для хранения значений согнутого и разогнутого состояния пальцев для сервпориводов
//переменные max*** для хранения значения при котором палец согнут
//переменные min*** для хранения значения при котором палец разогнут
int maxbfs = 70, minbfs = 150; //вышеуказанные значения для сгиба большого пальца
int maxbfr = 60, minbfr = 160; //вышеуказанные значения для поворота большого пальца
int maxuf = 180, minuf = 55; //вышеуказанные значения для сгиба указательного пальца
int maxltlf = 0, minltlf = 120; //вышеуказанные значения для сгиба мизинца
int maxf2 = 180, minf2 = 40; //вышеуказанные значения для сгиба среднего и безымянного пальцев

unsigned long svetimer = 0;  //переменная для моргания светодиода в режиме автоподстройки
const int a = 100; //переменная, отвечающая за количество опросов датчика для среднеарифметического способа фильтрации
byte left[a]; //массив, хранящий значения левой руки за 150 опросов (переменная a) для среднеарифметического способа фильтрации
byte right[a]; //массив, хранящий значения правой руки за 150 опросов (переменная a) для среднеарифметического способа фильтрации
int maLeft, miLeft, ampLeft; //переменные отвечающие за хранение максимального, минимального и среднего значения из массива left
int maRight, miRight, ampRight; //переменные отвечающие за хранение максимального, минимального и среднего значения из массива right

long t_choice; //переменная, отвечающая за хранение значения функции millis()
long duration, cm; //вспомогательные переменные для работы УЗ датчика, отвечающие за время возвращения сигнала с TRIG и итоговое расстояние до оъекта соответственно
int alarm_delay = 0, flagalarm = 0; //флаговые вспомогательные переменные для работы фунцкии alarm
unsigned long t; //переменная для хранения значения функции millis

boolean buttLeftWasUp = true; //флаговая переменная для хранения предыдущего значения с левой кнопки
boolean buttRightWasUp = true; //флаговая переменная для хранения предыдущего значения с правой кнопки
int clicksleft = 0, clicksright = 0; //переменные хранящие число нажатий для правой и для левой кнопки
int podstr_left = 255, podstr_right = 255; //подстроечные значения(изначально в максимуме)
int auto_flag = 0; //флаговая переменная отвечающая за срабатывание автоподстройки единожды
long t_podstr; //переменная хранящая millis() 

//функция возвращающая количество нажатий нажатий на левую кнопку(если нажимали на правую кнопку то возвращает 0)
int butt_left_clicks(int buttright){
  if(buttright == 0){ //если на правую кнопку не нажимали
    if (buttLeftWasUp && digitalRead(LEFT_BUTT)){
      delay(10);
      if (digitalRead(LEFT_BUTT))
        if (clicksleft < 2)
          clicksleft++;//то в конечном итоге увеличиваем число нажатий на левую кнопку (до двух нажатий)
    }
    buttLeftWasUp = !digitalRead(LEFT_BUTT); 
  }
  else clicksleft = 0;
  return clicksleft;
}

//функция возвращающая количество нажатий нажатий на левую кнопку(если нажимали на правую кнопку то возвращает 0)
int butt_right_clicks(void){
  if (buttRightWasUp && digitalRead(RIGHT_BUTT)){
    delay(10);
    if (digitalRead(RIGHT_BUTT))
      if(clicksright < 2)
        clicksright++;//то в конечном итоге увеличиваем число нажатий на левую кнопку (до двух нажатий)
  }
  buttRightWasUp = !digitalRead(RIGHT_BUTT);
  return clicksright;
}

//функция отвечающая за подстройку с помощью потенциометр
void podstroyka(int leftbutt){
  if (leftbutt == 0){    
    digitalWrite(RED_PODSTR, LOW);
    digitalWrite(YELLOW_PODSTR, LOW);
  }
  if (leftbutt == 1){ //если один раз нажали то подстраиваем левый порог
    podstr_left = analogRead(A3) / 4;
    digitalWrite(RED_PODSTR, HIGH);
  }
  if (leftbutt == 2){ //если один раз нажали то подстраиваем правый порог
    podstr_right = analogRead(A2) / 4;
    digitalWrite(YELLOW_PODSTR, HIGH);
  }
}

//функция отвечающая за автоподстройку
void auto_podstroyka(int rightbutt){
  if (rightbutt == 1){    
    digitalWrite(GREEN_PODSTR, HIGH);
  }
  if (rightbutt == 2 && auto_flag == 0){
    t_podstr = millis();
    int mxleft = 0;
    int mxright = 0;
    while (millis() < t_podstr + 3000){ //3 секунды анализируем сигнал и ищем максимальное значение
      int sign_left = sredn_filtr_left();
      int sign_right = sredn_filtr_right();
      if(sign_left > mxleft)
        mxleft = sign_left;
      if(sign_right > mxright)
        mxright = sign_right;
      if(millis()-svetimer>200){ //мигаем светодиодом
        svetimer=millis();
      }
      else if(millis()-svetimer>100){
        digitalWrite(GREEN_PODSTR,HIGH);
      }
      else if(millis()-svetimer>0){
        digitalWrite(GREEN_PODSTR,LOW);
      }
    }
    digitalWrite(GREEN_PODSTR,LOW);
    mxleft += 150; //увеличиваем максимальное значение на определенную величину
    mxright += 150;
    podstr_left = mxleft; //присваиваем подстроечному значению максимальное
    podstr_right = mxright;
    auto_flag = 1;    
  }
}
//функция для определения дистанции при аварийном отключении
int alarm_dist(void) {
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);
  duration = pulseIn(PIN_ECHO, HIGH, 4500);
  cm = (duration * 331 * 100) / 2000000; //итоговое расстояние до объекта в сантиметрах
  return cm;
}

//функция, отвечающая за экстренное отключение устройства
void alarm(int dist, int flag) {
  if (flag == 1) { //если находимся в состоянии жеста ОК, то:
    if (dist <= 5 && dist >= 1) { //если дистанция меньше 5см то зажигаем красный светодиод и переходим в знак Пистолетик
      PISTOL_sign();
      digitalWrite(RED, HIGH);
      alarm_delay = 1; //изменяем значение переменной, отвечающей за вкдючение светодиода после паузы
      delay (5000);
    } else if (dist <= 10 && dist >= 5) { //если дистанция меньше 10см то моргаем красным светодиодом и учащаем пики зуммера
      tone(ZUMM, 2000, 50);
      digitalWrite(RED, HIGH);
      delay(10);
      digitalWrite(RED, LOW);
      delay(10);
      tone(ZUMM, 2000, 50);
    } else if (dist <= 15 && dist >= 10) { //если дистанция меньше 15см то включаем пики зуммера
      tone(ZUMM, 2000, 300);
      delay(300);
      tone(ZUMM, 2000, 300);
    }
  }
}

//среднеарифметический фильтр для левой руки, возвращающий отфильтрованное значение
int sredn_filtr_left (void) {
  for (int i = 0; i < a; i++) {
    left[i] = map(analogRead(A0), 0, 1023, 0, 255); //записываем в массив left несколько значений с датчика (переменная a), отвечающего за левую руку
    delayMicroseconds(300);
  }
  miLeft = left[0];
  for (int i = 0; i < a; i++) {
    if (miLeft >= left[i]) //находим минимальное значение массива left
      miLeft = left[i];
  }
  maLeft = left[0];
  for (int i = 0; i < a; i++) {
    if (maLeft <= left[i]) //находим максимальное значение массива left
      maLeft = left[i];
  }
  ampLeft = maLeft - miLeft;  //вычисляем среднее значение за 150 опросов
  return ampLeft;
}

//среднеарифметический фильтр для правой руки, возвращающий отфильтрованное значение
int sredn_filtr_right (void) {
  for (int i = 0; i < a; i++) {
    right[i] = map(analogRead(A1), 0, 1023, 0, 255); //записываем в массив right несколько значений с датчика (переменная a), отвечающего за правую руку
    delayMicroseconds(300);
  }
  miRight = right[0];
  for (int i = 0; i < a; i++) {
    if (miRight >= right[i])
      miRight = right[i]; //находим минимальное значение массива right
  }
  maRight = right[0];
  for (int i = 0; i < a; i++) {
    if (maRight <= right[i])
      maRight = right[i]; //находим максимальное значение массива right
  }
  ampRight = maRight - miRight; //вычисляем среднее значение за 150 опросов
  return ampRight;
}

//функция, опрашивающая датчики за определенный период и определяющая какие руки напряжены
//если напряжена левая - возвращает 1, если правая - 2, если напряжены обе руки - 3
int choice_hands (void) {
  t_choice = millis();
  int flag_choice = 0;
  while ((millis() - t_choice) < 1500) {
    if (sredn_filtr_left() > podstr_left)
      flag_choice |= 1;
    if (sredn_filtr_right() > podstr_right)
      flag_choice |= 2;
  }
  return flag_choice;
}

//функция, отвечающая за смену жестов в завимисимости от значения, которое возвращает предыдущая функция
void choice_sign (int flag) {
  if (flag == 1) { //если choice_hands вернула 1, то показываем ОК и разрешаем функции alarm работать
    OK_sign();
    flagalarm = 1;
  }
  if (flag == 2) {
    GOAT_sign();  //если choice_hands вернула 2, то показываем Козу и запрещаем функции alarm работать (flagalarm = 0)
    flagalarm = 0;
  }
  if (flag == 3) {
    PISTOL_sign(); //если choice_hands вернула 3, то показываем Пистолетик и запрещаем функции alarm работать (flagalarm = 0)
    flagalarm = 0;
  }
}


//функция, отвечающая за поворот сервоприводов в зависимости от поступающих в нее значений
//если поступает 1 - согнуть палец, если поступает 0 - разогнуть палец
//функция создана для удобства написания программы (функциональный стиль)
void serv (int bfs, int bfr, int uf, int ltlf, int f2) {
  //для сгиба большого пальца
  if (bfs == 1)
    bigfingSgib.write(maxbfs);
  else
    bigfingSgib.write(minbfs);
  //для поворота большого пальца
  if (bfr == 1)
    bigfingRotat.write(maxbfr);
  else
    bigfingRotat.write(minbfr);
  //для сгиба указательного пальца
  if (uf == 1)
    ukazfing.write(maxuf);
  else
    ukazfing.write(minuf);
  //для сгиба мизинца
  if (ltlf == 1)
    littlefing.write(maxltlf);
  else
    littlefing.write(minltlf);
  //для сгиба среднего и безымянного пальцев
  if (f2 == 1)
    fing2.write(maxf2);
  else
    fing2.write(minf2);
}

//функция, задающая параметры для функции serv, формирующая знак OK
void OK_sign (void) {
  digitalWrite(GREEN, HIGH);
  digitalWrite(YELLOW, LOW);
  digitalWrite(RED, LOW);
  serv(1, 1, 1, 0, 0);
}

//функция, задающая параметры для функции serv, формирующая знак Коза
void GOAT_sign (void) {
  digitalWrite(GREEN, LOW);
  digitalWrite(YELLOW, HIGH);
  digitalWrite(RED, LOW);
  serv(1, 1, 0, 0, 1);
}

//функция, задающая параметры для функции serv, формирующая знак Пистолетик
void PISTOL_sign (void) {
  digitalWrite(GREEN, LOW);
  digitalWrite(YELLOW, LOW);
  digitalWrite(RED, HIGH);
  serv(0, 0, 0, 1, 1);
}

void setup() {
  //прописываем пины для сервпориводов
  Serial.begin(115200);
  bigfingSgib.attach(4);
  bigfingRotat.attach(5);
  ukazfing.attach(6);
  littlefing.attach(13);
  fing2.attach(8);
  digitalWrite(0, LOW);
  digitalWrite(1, LOW);
  //прописываем пины для светодиодов, зуммера и УЗ датчика
  pinMode(GREEN, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(ZUMM, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, INPUT);
  pinMode(7, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);  

  serv(0, 0, 0, 0, 0); //первым (стартовым) жестом показываем ладонь
  delay(500);
}

void loop() {
  //при помощи флаговой переменной отключаем красный светодиод после срабатывания аварийного отключения
  if (alarm_delay == 1) {
    digitalWrite(RED, LOW);
    alarm_delay = 0;
    flagalarm = 0;
  }
  podstroyka(butt_left_clicks(butt_right_clicks()));
  auto_podstroyka(butt_right_clicks());

  Serial.print(sredn_filtr_left());
  Serial.print(',');
  Serial.print(sredn_filtr_right());
  Serial.print(',');
  Serial.print(podstr_left);
  Serial.print(',');
  Serial.println(podstr_right);
  
  //choice_sign(choice_hands()); //вызываем функции для смена знаков
  //alarm(alarm_dist(), flagalarm); //вызываем функции для аварийного отключения
}
