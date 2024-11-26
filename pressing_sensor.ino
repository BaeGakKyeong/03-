// LCD 설정 (I2C 주소 0x27, 20x4 크기) - 주소 확인 필수
LiquidCrystal_I2C lcd(0x27, 20, 4);

// LCD 초기화
lcd.begin(20, 4);
lcd.backlight();
lcd.setCursor(0, 0);
lcd.print("System Starting...");

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