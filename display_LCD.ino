// 센서 값 읽기
int brakeValue = analogRead (brakeSensorPin );
int accelValue = analogRead (accelSensorPin );

  // 데이터 기록 (데이터 기록 코드 부분 – 김채환)
File dataFile = SD .open ("log.txt", FILE_WRITE );
    if (dataFile ){
        dataFile .print (now .year (), DEC );
        dataFile .print ('/');
        dataFile .print (now .month (), DEC );
        dataFile .print ('/');
        dataFile .print (now .day (), DEC );
        dataFile .print (" ");
        dataFile .print (now .hour (), DEC );
        dataFile .print (':');
        dataFile .print (now .minute (), DEC );
        dataFile .print (':');
        dataFile .print (now .second (), DEC );
        dataFile .print (", Brake: ");
        dataFile .print (brakeValue );
        dataFile .print (", Accelerator: ");
        dataFile .println (accelValue );
        dataFile .close ();
        Serial .println ("데이터가 log.txt에 기록되었습니다.");
    }
    else {
        Serial .println ("log.txt 파일 열기 실패");
    }