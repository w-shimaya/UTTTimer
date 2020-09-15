#define SET(n) digitalWrite((n), LOW)
#define READY 0
#define COUNT 1
#define STOP 2
#define PUSH_SHORT 100

// ピン配置
const int dig[4] = {7, 6, 5, 4};

const int a = 3;
const int b = 1;
const int c = A3;
const int d = A1;
const int e = A0;
const int f = 2;
const int g = A4;
const int dp = A2;

const int spk = 10;

const int sw_one = 8;
const int sw_two = 9;
// ここまで

unsigned long time_milli_start = 0; //カウントが始まった時刻
unsigned long time_milli_now = 0; //表示する時刻
unsigned long time_milli_stop = 0; //停止時刻
stack<unsigned long> alarm_start_time; // ブザーの開始時間

// buzzer_state: ブザーがどのパターンか．
typedef enum { Five, Ten, Twenty, Button } BuzzerState;
// 空なら無音
stack<BuzzerState> buzzer_state;

int state; // タイマーの状態

int count_low_one = 0; // チャタリングに使うカウント
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
  // repeatを超える回数は鳴らさない
  if (now_repeats > repeats) {
    digitalWrite(spk, LOW);
    return 0;
  }

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
  time_milli_now = millis();
  time_milli_stop = 0;

  // タイマーの状態
  state = READY;
}

void show_num(int* n) {
  // nは各桁の数字が要素の配列（の先頭ポインタ）
  // ディスプレイにnで指定される数字を表示
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
     delayMicroseconds(60);
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
        buzzer_state.push(Button);
        alarm_start_time.push(millis());
        state = COUNT;
      }
      break;

    case COUNT: //COUNTのとき，カウントアップ
      time_milli_now = millis() - time_milli_start + time_milli_stop; // display
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
        buzzer_state.push(Button);
        alarm_start_time.push(millis());
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
        buzzer_state.push(Button);
        alarm_start_time.push(millis());
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
        buzzer_state.push(Button);
        alarm_start_time.push(millis());
      }

      break;
  }
 
  // 時間お知らせブザー
  if (state == COUNT) {
    /*
     * 1200n -> 20
     * 600n -> 10
     * 300n -> 5
     */
    if (time_milli_now > 1) {
      if (time_milli_now % 120000 == 0) {
        buzzer_state.push(Twenty);
        alarm_start_time.push(millis());
      } else if (time_milli_now % 60000 == 0) {
        buzzer_state.push(Ten);
        alarm_start_time.push(millis());
      } else if (time_milli_now % 30000 == 0) {
        buzzer_state.push(Five);
        alarm_start_time.push(millis());
      }
    }
  }
  
  // 音を鳴らす
  if (!buzzer_state.empty()) {
    int remainder = 0;
    switch (buzzer_state.top()) {
      case Five:
        remainder = buzzer(200, 200, 2, millis() - alarm_start_time.top());
        break;
      case Ten:
        remainder = buzzer(200, 200, 3, millis() - alarm_start_time.top());
        break;
      case Twenty:
        remainder = buzzer(200, 200, 6, millis() - alarm_start_time.top());
        break;
      case Button:
        remainder = buzzer(200, 200, 1, millis() - alarm_start_time.top());
        break;
    }
  
    // 終わったらブザー状態を一つ戻す
    if (remainder == 0) {
      digitalWrite(spk, LOW);
      buzzer_state.pop();
      alarm_start_time.pop();
    }
  }

  // 4桁に収まらなくなったとき（要検討）
  if (delta >= 100 * 60) {
    time_milli_start = millis();
    time_milli_stop = 0;
  }
}
