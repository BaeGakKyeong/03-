// 센서 값 읽기
int brakeValue = analogRead (brakeSensorPin );
int accelValue = analogRead (accelSensorPin );

  // LCD에 데이터 표시
lcd .clear ();  // 깜박거림 방지를 위해 조건부로 변경 가능
lcd .setCursor (0 , 0 );
lcd .print ("Brake: " + String (brakeValue ));
lcd .setCursor (0 , 1 );
lcd .print ("Accel: " + String (accelValue ));
lcd .setCursor (0 , 2 );
lcd .print ("Time: ");    
lcd .print (now .hour ());
lcd .print (":");
lcd .print (now .minute ());
lcd .print (":");
lcd .print (now .second ());

delay (1000 );  // 1초 대기