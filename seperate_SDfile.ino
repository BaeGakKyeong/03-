// 파일 이름
char fileName[16];
void setup() {
  Serial.begin(9600); // 시리얼 통신 시작
-------------------------------------------------------------------------
void createNewFileName() {
  DateTime now = rtc.now(); // 현재 시간 읽기
  snprintf(fileName, sizeof(fileName), "%02d%02d%02d%02d.TXT", 
           now.month(), now.day(), now.hour(), now.minute());
}