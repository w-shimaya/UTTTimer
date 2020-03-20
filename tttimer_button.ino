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
  pinMode(dig[0], OUTPUT);
  pinMode(dig[1], OUTPUT);
  pinMode(dig[2], OUTPUT);
  pinMode(dig[3], OUTPUT);
  pinMode(a, OUTPUT);
  pinMode(b, OUTPUT);
  pinMode(c, OUTPUT);
  pinMode(d, OUTPUT);
  pinMode(e, OUTPUT);
  pinMode(f, OUTPUT);
  pinMode(g, OUTPUT);
  pinMode(dp, OUTPUT);
  pinMode(spk, OUTPUT);

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



//on_timeミリ秒鳴らしてoff_timeミリ秒無音，これをrepeat回繰り返す
//alarm_start_timeは0以外の時に動作するようになっており，alarm()は毎ループ呼んでおいて，alarm_start_timeに入れる値で制御する
void alarm(int on_time,int off_time, int repeat, unsigned long alarm_start_time){
  int alarm_elapsed_time = millis() - alarm_start_time;
  for (int i=0,i<repeat,i++){
    if(alarm_elapsed_time < (on_time+off_time) * i + on_time){
      digitalWrite(spk,HIGH);
      break;
    }
    else if(alarm_elapsed_time < (on_time+off_time)*(i+1)){
      digitalWrite(spk,LOW);
      break;
    }
  }
  if (alarm_elapsed_time >= (on_time+off_time)*repeat)alarm_start_time = 0;
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


  if ((delta % 300 == 0) && (delta > 1)){
    if (alarm_start_time == 0){
      alarm_start_time = time_milli_end;
    }
  }

  alarm(200,50,4,alarm_start_time);

}
