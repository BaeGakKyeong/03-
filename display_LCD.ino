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