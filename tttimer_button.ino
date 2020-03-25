#define SET(n) digitalWrite((n), LOW)
#define READY 0
#define COUNT 1
#define STOP 2
#define PUSH_SHORT 100

const int dig[4] = {7, 6, 5, 4};

const int a = 1;
const int b = 3;
const int c = A1;
const int d = A3;
const int e = A4;
const int f = 2;
const int g = A0;
const int dp = A2;

const int spk = 10;

const int sw_one = 8;
const int sw_two = 9;


unsigned long time_milli_start = 0; //カウントが始まった時刻
unsigned long time_milli_now = 0; //表示する時刻
unsigned long time_milli_stop = 0; //停止時刻
unsigned long alarm_start_time = 0;
unsigned long prev_alarm_start_time = 0;

// buzzer_state: ブザーが鳴っているか，鳴っているならブザーがどのパターンか
typedef enum { Silent, Five, Ten, Twenty, Button } BuzzerState;
BuzzerState buzzer_state;
BuzzerState prev_buz_state;

int state = 0; //今の状態

int count_low_one = 0; //チャタリングに使うカウント
int count_low_two = 0;

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

/**
 * buzzer
 * 
 * Parameters
 * ----------
 * on_time : int
 *     繰り返し1回で音を鳴らす時間 [m sec]
 * off_time : int
 *     繰り返し1回で音を消す時間 [m sec]
 * repeats : int
 *     繰り返し回数
 * elapsed_time : int 
 *     鳴り始めからの経過時間 [m sec]
 * Returns
 * -------
 * int
 *     残り繰り返し回数（今鳴っている分は含まない）→ 0になったら鳴り終わり
 */
int buzzer(int on_time, int off_time, int repeats, int elapsed_time) {
  int now_repeats = elapsed_time / (on_time + off_time);
  int remainder = elapsed_time % (on_time + off_time);
  if (remainder < on_time) {
    digitalWrite(spk, HIGH);
  } else {
    digitalWrite(spk, LOW);
  }
  return repeats - now_repeats;
}

void setup() {
  // 7セグとブザーのピンを出力にする
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

  //スイッチのピンの設定
  pinMode(sw_one,INPUT_PULLUP);
  pinMode(sw_two,INPUT_PULLUP);

  // 全てのDIG(桁)をクリア
  clearDigit();

  // 全てのセグ(A-G,DP)をクリア
  clearSegment();

  time_milli_start = millis();

  // ブザーの状態
  buzzer_state = Silent;
  time_milli_now = millis();
  time_milli_stop = 0;

  state = READY;
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
       default:
         SET(a);
         break;
     }
     if (i == 1) SET(dp);
     delayMicroseconds(50);
  }
}



//on_timeミリ秒鳴らしてoff_timeミリ秒無音，これをrepeat回繰り返す
//alarm_start_timeは0以外の時に動作するようになっており，alarm()は毎ループ呼んでおいて，alarm_start_timeに入れる値で制御する
void alarm(int on_time,int off_time, int repeat, unsigned long alarm_start_time){
  if(alarm_start_time > 0){
    int alarm_elapsed_time = millis() - alarm_start_time;
    for (int i=0; i<repeat; i++){
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
}



void loop() {
  int num[4] = {};
  int delta = 0;
  int now_sec = 0;
  int now_min = 0;
  switch(state){
    case READY: //READYのとき，表示は全て０
      show_num(num);

      //スイッチ１(start/stop)が押されたとき，その時刻をtime_milli_startにしてCOUNTへ
      if (digitalRead(sw_one) == LOW){
        if(count_low_one <= PUSH_SHORT)count_low_one++;
      }else{
        count_low_one = 0;
      }
      if(count_low_one == PUSH_SHORT){
        time_milli_start = millis();
        state = COUNT;
      }
      break;

    case COUNT: //COUNTのとき，カウントアップ
      time_milli_now = millis() - time_milli_start + time_milli_stop;
      delta = time_milli_now / 1000; //second
      now_sec = delta % 60;
      now_min = delta / 60;

      num[0] = now_min / 10;
      num[1] = now_min % 10;
      num[2] = now_sec / 10;
      num[3] = now_sec % 10;
      show_num(num);

      //スイッチ１(start/stop)が押されたとき，表示されている時刻をtime_milli_stopに記録してSTOPへ
      if (digitalRead(sw_one) == LOW){
        if(count_low_one <= PUSH_SHORT)count_low_one++;
      }else{
        count_low_one = 0;
      }
      if(count_low_one == PUSH_SHORT){
        time_milli_stop = time_milli_now;
        state = STOP;
      }
      break;

    case STOP: //STOPのとき，カウント停止．time_milli_stopを表示
      delta = time_milli_stop / 1000;
      now_sec = delta % 60;
      now_min = delta / 60;

      num[0] = now_min / 10;
      num[1] = now_min % 10;
      num[2] = now_sec / 10;
      num[3] = now_sec % 10;
      show_num(num);

      //スイッチ1(start/stop)が押されたとき，その時刻をtime_milli_startにしてCOUNTへ
      if (digitalRead(sw_one) == LOW){
        if(count_low_one <= PUSH_SHORT)count_low_one++;
      }else{
        count_low_one = 0;
      }
      if(count_low_one == PUSH_SHORT){
        time_milli_start = millis();
        prev_buz_state = buzzer_state;
        buzzer_state = Button;
        state = COUNT;
      }

      //スイッチ2(reset)が押されたとき，time_milli_stopを0にしてREADYへ
      if (digitalRead(sw_two) == LOW){
        if(count_low_two <= PUSH_SHORT)count_low_two++;
      }else{
        count_low_two = 0;
      }
      if(count_low_two == PUSH_SHORT){
        time_milli_stop = 0;
        state = READY;
      }

      break;
  }
  
  /*
   * 1200n -> 20
   * 600n -> 10
   * 300n -> 5
   */
  if (delta > 1) {
    if (delta % 1200 == 0) {
      if (buzzer_state == Silent) {
        buzzer_state = Twenty;
        alarm_start_time = time_milli_now;
      } else if (buzzer_state == Button) {
        prev_buz_state = Twenty;
        prev_alarm_start_time = time_milli_now;
      }
    } else if (delta % 600 == 0) {
      if (buzzer_state == Silent) {
        buzzer_state = Ten;
        alarm_start_time = time_milli_now;
      } else if (buzzer_state == Button) {
        prev_buz_state = Ten;
        prev_alarm_start_time = time_milli_now;
      }
    } else if (delta % 300 == 0) {
      if (buzzer_state == Silent) {
        buzzer_state = Five;
        alarm_start_time = time_milli_now;
      } else if (buzzer_state == Button) {
        prev_buz_state = Five;
        prev_alarm_start_time = time_milli_now;
      }
    }
  }

  // Silentじゃなければ鳴らす
  int remainder = 0;
  switch (buzzer_state) {
    case Silent:
      break;
    case Five:
      remainder = buzzer(50, 50, 2, time_milli_now - alarm_start_time);
      break;
    case Ten:
      remainder = buzzer(50, 50, 3, time_milli_now - alarm_start_time);
      break;
    case Twenty:
      remainder = buzzer(100, 50, 6, time_milli_now - alarm_start_time);
    case Button:
      remainder = buzzer(50, 50, 1, time_milli_now - alarm_start_time);
      break;
  }
  // 終わったら待機
  if (remainder == 0) {
    digitalWrite(spk, LOW);
    if (buzzer_state == Button) {
      buzzer_state = prev_buz_state;
      alarm_start_time = prev_alarm_start_time;
    } else {
      buzzer_state = Silent;
    }
  }
}
