#include <Wire.h >               // I2C 통신 라이브러리
#include <RTClib.h >             // RTC 제어 라이브러리
#include <SPI.h >                // SPI 통신 라이브러리
#include <SD.h >                 // SD 카드 라이브러리
#include <LiquidCrystal_I2C.h >  // I2C LCD 제어 라이브러리

// RTC, SD 카드, LCD 설정
RTC_DS3231 rtc;
const int brakeSensorPin = A0;  // 브레이크 센서 핀
const int accelSensorPin = A1;  // 악셀 센서 핀
const int chipSelect =4;       // SD 카드 모듈 CS 핀

// 압력 센서 선형화 상수 (FSR-402를 기반으로 설정)
const float R_fixed =10000.0;  // 고정 저항 값 (10k 옴)
const float A =30000.0;        // 압력 상수 (센서 데이터시트 기반 비례 상수)
const float B =1.4;            // 압력 상수 (센서 데이터시트 기반 지수 상수)

// LCD 설정 (I2C 주소 0x27, 20x4 크기) - 주소 확인 필수
LiquidCrystal_I2C lcd(0x27, 20, 4);

// 마지막 초기화 날짜
DateTime lastInitDate;
void setup() {
  Serial.begin(9600);  // 시리얼 통신 시작

  // RTC 초기화
  if (!rtc.begin()) {
    Serial.println("RTC 초기화 실패!");
    while (1);  // RTC 오류 시 멈춤
  }

  // 기본 시간 설정
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  // 기본 시간 설정
  Serial.println("RTC 정상 실행 중");

  // SD 카드 초기화
  if (!SD.begin(chipSelect)) {
    Serial.println("SD 카드 초기화 실패!");
    while (1);  // SD 카드 오류 시 멈춤
  } else {
    Serial.println("SD 카드 초기화 성공!");
  }

  // LCD 초기화
  lcd.begin(20, 4);    // LCD의 열과 행을 설정합니다.
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("System Starting...");

  // 마지막 초기화 날짜 읽기
  File dateFile = SD.open("lastInitDate.txt", FILE_READ);
  if (dateFile) {
    Serial.println("lastInitDate.txt 파일 열기 성공");
    String dateStr = dateFile.readStringUntil('\n');
    unsigned long unixTime = dateStr.toInt();
    if (unixTime >0) {
      lastInitDate = DateTime(unixTime);  // 읽은 시간 변환
      Serial.println("파일에서 읽은 시간: "+ String(lastInitDate.year()) +"/"+
                     String(lastInitDate.month()) +"/"+ String(lastInitDate.day()));
    } else {
      Serial.println("유닉스 시간 변환 오류, 현재 시간으로 초기화");
      lastInitDate = rtc.now();
      resetSDCard();  // SD 카드 재설정
    }
    dateFile.close();
  } else {
    Serial.println("lastInitDate.txt 파일 없음, 새로운 파일 생성 중...");
    lastInitDate = rtc.now();
    resetSDCard();  // 새로운 초기화 날짜 저장
  }
}
void loop() {
  DateTime now = rtc.now();  // 현재 시간 읽기

  // 7일이 지났는지 확인
  if ((now.unixtime() - lastInitDate.unixtime()) >=7 *24 *60 *60) {
    resetSDCard();
    lastInitDate = now;  // 현재 날짜로 업데이트
  }

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
  File dataFile = SD.open("log.txt", FILE_WRITE);
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
    Serial.println("데이터가 log.txt에 기록되었습니다.");
  } else {
    Serial.println("log.txt 파일 열기 실패");
  }

  // LCD에 데이터 표시
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Brake: "+ String(brakePressure) +" ");
  lcd.setCursor(0, 1);
  lcd.print("Accel: "+ String(accelPressure) +" ");
  lcd.setCursor(0, 2);
  lcd.print("Time: ");
  lcd.print(now.hour());
  lcd.print(":");
  lcd.print(now.minute());
  lcd.print(":");
  lcd.print(now.second());
  delay(1000);  // 1초 대기
}
void resetSDCard() {

  // 로그 파일 삭제
  if (SD.exists("log.txt")) {
    SD.remove("log.txt");
    Serial.println("log.txt 파일 삭제 완료");
  }

  // 새로운 날짜 파일 작성
  File dateFile = SD.open("lastInitDate.txt", FILE_WRITE);
  if (dateFile) {
    dateFile.println(rtc.now().unixtime());
    dateFile.close();
    Serial.println("lastInitDate.txt 파일에 날짜 저장 완료");
  } else {
    Serial.println("lastInitDate.txt 파일 열기 실패");
  }
}