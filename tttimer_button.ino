#define SET(n) digitalWrite((n), LOW)

const int dig[4] = {2, 5, 6, 14};

const int a = 3;
const int b = 7;
const int c = 16;
const int d = 18;
const int e = 19;
const int f = 4;
const int g = 15;
const int dp = 17;

const int spk = 10;

unsigned long time_milli_start = 0;
unsigned long time_milli_end = 0;
unsigned long alarm_start_time = 0;

// 全てのDIG(桁)をクリア
void clearDigit(){
  for(int i = 0; i < 4; i++){
    digitalWrite(dig[i],LOW);
  }
}
// 全てのセグ(A-G,DP)をクリア
void clearSegment(){
  digitalWrite(a,HIGH);
  digitalWrite(b,HIGH);
  digitalWrite(c,HIGH);
  digitalWrite(d,HIGH);
  digitalWrite(e,HIGH);
  digitalWrite(f,HIGH);
  digitalWrite(g,HIGH);
  digitalWrite(dp,HIGH);
}

void setup() {
  // 全てのピンを出力にする
  for(int i =0; i < 13; i++){
    pinMode((i+1),OUTPUT);
  }

  // 全てのDIG(桁)をクリア
  clearDigit();

  // 全てのセグ(A-G,DP)をクリア
  clearSegment();

  time_milli_start = millis();
  time_milli_end = millis();
}

void show_num(int* n) {
  // nは各桁の数字が要素の配列（の先頭ポインタ）
  for (int i = 0; i < 4; ++i) {
    clearSegment(); clearDigit();
    digitalWrite(dig[i], HIGH);
    switch (n[i]) {
      case 0:
         SET(a); SET(b); SET(c); SET(d); SET(e); SET(f);
         break;
       case 1:
         SET(b); SET(c);
         break;
       case 2:
         SET(a); SET(b); SET(g); SET(e); SET(d);
         break;
       case 3:
         SET(a); SET(b); SET(g); SET(c); SET(d);
         break;
       case 4:
         SET(f); SET(g); SET(b); SET(c);
         break;
       case 5:
         SET(a); SET(f); SET(g); SET(c); SET(d);
         break;
       case 6:
         SET(a); SET(f); SET(g); SET(e); SET(c); SET(d);
         break;
       case 7:
         SET(a); SET(b); SET(c); SET(f);
         break;
       case 8:
         SET(a); SET(b); SET(c); SET(d); SET(e); SET(f); SET(g);
         break;
       case 9:
         SET(a); SET(b); SET(c); SET(d); SET(f); SET(g);
         break;
     }
     if (i == 1) SET(dp);
     delayMicroseconds(50);
  }
}

void alarm_on(){
  digitalWrite(spk,HIGH);
}

void alarm_off(){
  digitalWrite(spk,LOW);
}


void loop() {
  time_milli_end = millis();
  int delta = (time_milli_end - time_milli_start) / 1000; // second
  int now_sec = delta % 60;
  int now_min = delta / 60;

  int num[4];
  num[0] = now_min / 10;
  num[1] = now_min % 10;
  num[2] = now_sec / 10;
  num[3] = now_sec % 10;
  show_num(num);

//300秒ごとに，200ミリ秒鳴って50ミリ秒無音を1秒間
  if((delta % 300 == 0) && (delta > 1)){
    if (alarm_start_time == 0){
      alarm_on();
      alarm_start_time = time_milli_end;
    }else{
      int alarm_elapsed_time = time_milli_end - alarm_start_time;
      if (alarm_elapsed_time < 200)alarm_on();
      else if (alarm_elapsed_time >= 200 && alarm_elapsed_time < 250)alarm_off();
      else if (alarm_elapsed_time >= 250 && alarm_elapsed_time < 450)alarm_on();
      else if (alarm_elapsed_time >= 450 && alarm_elapsed_time < 500)alarm_off();
      else if (alarm_elapsed_time >= 500 && alarm_elapsed_time < 700)alarm_on();
      else if (alarm_elapsed_time >= 700 && alarm_elapsed_time < 750)alarm_off();
      else if (alarm_elapsed_time >= 750 && alarm_elapsed_time < 950)alarm_on();
      else if (alarm_elapsed_time >= 950 && alarm_elapsed_time < 1000)alarm_off();
    }
  }else if (delta % 300 == 1)alarm_start_time = 0;
}
