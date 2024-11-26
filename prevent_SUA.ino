#include <Wire.h >// I2C 통신 라이브러리
#include <RTClib.h >// RTC 제어 라이브러리
#include <SPI.h >// SPI 통신 라이브러리
#include <SD.h >// SD 카드 라이브러리
#include <LiquidCrystal_I2C.h >// I2C LCD 제어 라이브러리

// RTC, SD 카드, LCD 설정
RTC_DS3231 rtc;
const int brakeSensorPin = A0; // 브레이크 센서 핀
const int accelSensorPin = A1; // 악셀 센서 핀
const int chipSelect =4; // SD 카드 모듈 CS 핀

// 압력 센서 선형화 상수 (FSR-402를 기반으로 설정)
const float R_fixed =10000.0; // 고정 저항 값 (10k 옴)
const float A =30000.0; // 압력 상수 (센서 데이터시트 기반 비례 상수)
const float B =1.4; // 압력 상수 (센서 데이터시트 기반 지수 상수)
// LCD 설정 (I2C 주소 0x27, 20x4 크기) - 주소 확인 필수
LiquidCrystal_I2C lcd(0x27, 20, 4);

// 파일 이름
char fileName[16];
void setup() {
  Serial.begin(9600); // 시리얼 통신 시작

  // RTC 초기화
  if (!rtc.begin()) {
    Serial.println("RTC 초기화 실패!");
    while (1); // RTC 오류 시 멈춤
  }

  // SD 카드 초기화
  if (!SD.begin(chipSelect)) {
    Serial.println("SD 카드 초기화 실패!");
    while (1); // SD 카드 오류 시 멈춤
  } else {
    Serial.println("SD 카드 초기화 성공!");
  }

  // LCD 초기화
  lcd.begin(20, 4);
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("System Starting...");

  // 새로운 파일 이름 생성
  createNewFileName();
  Serial.print("새로운 파일 생성: ");
  Serial.println(fileName);
}

void loop() {
  DateTime now = rtc.now(); // 현재 시간 읽기

  // 센서 값 읽기
  int brakeRawValue = analogRead(brakeSensorPin);
  int accelRawValue = analogRead(accelSensorPin);

  // 압력 값 계산
  float brakeResistance = (1023.0 - brakeRawValue) * R_fixed / brakeRawValue;
  float accelResistance = (1023.0 - accelRawValue) * R_fixed / accelRawValue;
  float brakePressure = A * pow(brakeResistance, -B);
  float accelPressure = A * pow(accelResistance, -B);

  // 디버깅 메시지 출력
  Serial.print("Brake Pressure: ");
  Serial.print(brakePressure);
  Serial.print(" Pa, Accelerator Pressure: ");
  Serial.println(accelPressure);

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
    Serial.println("파일 열기 실패: ");
    Serial.println(fileName);
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
  delay(1000); // 1초 대기
}

void createNewFileName() {
  DateTime now = rtc.now(); // 현재 시간 읽기
  snprintf(fileName, sizeof(fileName), "%02d%02d%02d%02d.TXT", 
           now.month(), now.day(), now.hour(), now.minute());
}