#define SET(n) digitalWrite((n), LOW)

const int dig[4] = {1, 2, 3, 4};
 
const int a = 12;
const int b = 10;
const int c =  8;
const int d =  6;
const int e =  5;
const int f = 11;
const int g =  9;
const int dp = 7;

unsigned long time_milli_start = 0;
unsigned long time_milli_end = 0;
 
// 全てのDIG(桁)をクリア
void clearDigit(){
  for(int i = 0; i < 4; i++){
    digitalWrite((i+1),LOW);
  }  
}
// 全てのセグ(A-G,DP)をクリア
void clearSegment(){
  for(int i = 5; i < 13; i++){
    digitalWrite(i,HIGH);
  }  
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
}
