#include <Wire.h >// I2C 통신 라이브러리
#include <RTClib.h >// RTC 제어 라이브러리
#include <SPI.h >// SPI 통신 라이브러리
#include <SD.h >// SD 카드 라이브러리
#include <LiquidCrystal_I2C.h >// I2C LCD 제어 라이브러리

RTC_DS3231 rtc;
const int brakeSensorPin = A0; // 브레이크 센서 핀
const int accelSensorPin = A1; // 악셀 센서 핀
const int chipSelect =4; // SD 카드 모듈 CS 핀

const float R_fixed =10000.0; // 고정 저항 값 (10k 옴)
const float A =30000.0; // 압력 상수
const float B =1.4; // 압력 상수

LiquidCrystal_I2C lcd(0x27, 20, 4); // LCD 설정
char fileName[32]; // 고유 파일 이름 저장

void setup() {
  Serial.begin(9600);

  // RTC 초기화
  if (!rtc.begin()) {
    Serial.println("RTC 초기화 실패!");
    while (1);
  }

  // SD 카드 초기화
  if (!SD.begin(chipSelect)) {
    Serial.println("SD 카드 초기화 실패! SD 모듈 연결 및 포맷 확인 필요");
    while (1);
  } else {
    Serial.println("SD 카드 초기화 성공!");
  }

  // LCD 초기화
  lcd.begin(20, 4);
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("System Starting...");

  // 전원 재인가 시 새로운 파일 이름 생성
  createNewFileName();
  Serial.print("새로운 파일 생성: ");
  Serial.println(fileName);
  Serial.println("시스템 초기화 완료");
}

void loop() {
  DateTime now = rtc.now();

  // 센서 값 읽기
  int brakeRawValue = analogRead(brakeSensorPin);
  int accelRawValue = analogRead(accelSensorPin);

  // 압력 값 계산
  float brakeResistance = (1023.0 - brakeRawValue) * R_fixed / brakeRawValue;
  float accelResistance = (1023.0 - accelRawValue) * R_fixed / accelRawValue;
  float brakePressure = A * pow(brakeResistance, -B);
  float accelPressure = A * pow(accelResistance, -B);

  // 데이터 기록
  File dataFile = SD.open(fileName, FILE_WRITE);
  if (dataFile) {
    dataFile.print(now.year(), DEC);
    dataFile.print('/');
    dataFile.print(now.month(), DEC);
    dataFile.print('/');
    dataFile.print(now.day(), DEC);
    dataFile.print(" ");
    dataFile.print(now.hour(), DEC);
    dataFile.print(':');
    dataFile.print(now.minute(), DEC);
    dataFile.print(':');
    dataFile.print(now.second(), DEC);
    dataFile.print(", Brake: ");
    dataFile.print(brakePressure);
    dataFile.print(" Pa, Accelerator: ");
    dataFile.println(accelPressure);
    dataFile.close();
    Serial.println("데이터가 파일에 기록되었습니다.");
  } else {
    Serial.println("파일 열기 실패!");
  }

  // LCD에 데이터 표시
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Brake: "+ String(brakePressure) +" Pa");
  lcd.setCursor(0, 1);
  lcd.print("Accel: "+ String(accelPressure) +" Pa");
  lcd.setCursor(0, 2);
  lcd.print("Time: ");
  lcd.print(now.hour());
  lcd.print(":");
  lcd.print(now.minute());
  lcd.print(":");
  lcd.print(now.second());
  delay(1000);
}
void createNewFileName() {
  DateTime now = rtc.now();
  snprintf(fileName, sizeof(fileName), "%04d%02d%02d_%02d%02d%02d.TXT",
           now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());
}