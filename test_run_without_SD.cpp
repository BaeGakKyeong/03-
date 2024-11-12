#include <Wire.h>               // I2C 통신 라이브러리
#include <RTClib.h>             // DS3231 RTC 제어 라이브러리
#include <LiquidCrystal_I2C.h>  // I2C LCD 제어 라이브러리

// RTC, LCD 설정
RTC_DS3231 rtc;
const int fsr1Pin = A0;         // FSR-402 센서 1 핀
const int fsr2Pin = A1;         // FSR-402 센서 2 핀

// LCD 설정 (I2C 주소는 실제 주소로 변경 필요)
LiquidCrystal_I2C lcd(0x27, 20, 4);  // I2C 주소가 0x27이 아닐 경우 확인한 주소로 변경

void setup() {
  Serial.begin(9600);  // 시리얼 통신 시작

  // RTC 초기화
  if (!rtc.begin()) {
    Serial.println("RTC initialization failed!");
    while (1);  // RTC 오류 시 멈춤
  }

  // RTC 시간을 초기화 (필요할 경우, 처음 설정 시만 실행)
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  // 컴파일된 시간으로 RTC 설정

  // LCD 초기화
  lcd.begin(20, 4);    // LCD의 열과 행을 설정합니다.
  lcd.backlight();      // 백라이트 켜기
  lcd.setCursor(0, 0);
  lcd.print("System Starting...");
  delay(2000);          // 초기화 메시지를 볼 수 있도록 2초 대기
}

void loop() {
  DateTime now = rtc.now();  // 현재 시간 읽기

  // FSR 센서 값 읽기
  int fsr1Value = analogRead(fsr1Pin);
  int fsr2Value = analogRead(fsr2Pin);

  // LCD에 데이터 표시
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("FSR1: " + String(fsr1Value));
  lcd.setCursor(0, 1);
  lcd.print("FSR2: " + String(fsr2Value));
  lcd.setCursor(0, 2);
  lcd.print("Time: ");
  lcd.print(now.hour());
  lcd.print(":");
  lcd.print(now.minute());
  lcd.print(":");
  lcd.print(now.second());

  delay(1000);  // 1초 대기
}
