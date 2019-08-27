#include <Wire.h>
#include "Adafruit_HTU21DF.h"
#include <LiquidCrystal.h>

// Connect Vin to 3-5VDC
// Connect GND to ground
// Connect SCL to I2C clock pin (A5 on UNO)
// Connect SDA to I2C data pin (A4 on UNO)

Adafruit_HTU21DF htu = Adafruit_HTU21DF();
	const int Max = 10;
	const int Ave = Max - 1;
	const int Min = Max - 2;
	float TempRA[Max + 1]; 
	float HumidityRA[Max + 1];
	int index = 0;
	const int rs = 7, en = 8, d4 = 9, d5 = 10, d6 = 11, d7 = 12;
	LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

	
void TakeReading(){
	float AvgTemp = 0;
	float AvgHumidity = 0;

	TempRA[index] = htu.readTemperature();
	HumidityRA[index] = htu.readHumidity();
	if (TempRA[index] < TempRA[Min]) {TempRA[Min] = TempRA[index]; Serial.println("New Min temp!");}  //If this reading is less than the current min, store it as the new min.
	if (TempRA[index] > TempRA[Max]) {TempRA[Max] = TempRA[index]; Serial.println("New Max temp!");}  //If this reading is greater than the current max, store it as the new max.
	if (HumidityRA[index] < HumidityRA[Min]) {HumidityRA[Min] = HumidityRA[index]; Serial.println("New Min Humidity!");}  //If this reading is less than the current min, store it as the new min.
	if (HumidityRA[index] > HumidityRA[Max]) {HumidityRA[Max] = HumidityRA[index]; Serial.println("New Max Humidity!");}  //If this reading is greater than the current max, store it as the new max.

	if (index++ == 7)	{
		index = 0;
	}
	for (int x = 0; x <=7; x++){
		AvgTemp += TempRA[x];
		AvgHumidity += HumidityRA[x];
	}
	TempRA[Ave] = (AvgTemp / 8);
	HumidityRA[Ave] = (AvgHumidity / 8);
}


void setup() {
  Serial.begin(9600);
  Serial.println("HTU21D-F test");

  if (!htu.begin()) {
    Serial.println("Couldn't find sensor!");
    while (1);
  }
  	TempRA[Min] = htu.readTemperature();
	HumidityRA[Min] = htu.readHumidity();
	// set up the LCD's number of columns and rows:
	lcd.begin(16, 2);
	// Print a message to the LCD.
//	lcd.print("hello, world!");

}

void loop() {
	TakeReading();
	Serial.println(index - 1);
	Serial.print("Recent Temp: "); Serial.print(TempRA[index - 1]); Serial.print(" C");
	Serial.print("\t");
	Serial.print("Min Temp: "); Serial.print(TempRA[Min]); Serial.print(" C");
	Serial.print("\t");
	Serial.print("Average Temp: "); Serial.print(TempRA[Ave]); Serial.print(" C");
	Serial.print("\t");
	Serial.print("Max Temp: "); Serial.print(TempRA[Max]); Serial.print(" C");
	Serial.println("");
	Serial.print("Recent Humidity: "); Serial.print(HumidityRA[index - 1]); Serial.print(" C");
	Serial.print("\t");
	Serial.print("Min Humidity: "); Serial.print(HumidityRA[Min]); Serial.print(" C");
	Serial.print("\t");
	Serial.print("Average Humidity: "); Serial.print(HumidityRA[Ave]); Serial.print(" C");
	Serial.print("\t");
	Serial.print("Max Humidity: "); Serial.print(HumidityRA[Max]); Serial.print(" C");
	Serial.println("");
	// set the cursor to column 0, line 0
	lcd.setCursor(0, 0);
	lcd.print("T");
	lcd.print(TempRA[index]);
	lcd.setCursor(7,0);
	lcd.print("avg=");
	lcd.print(TempRA[Ave]);
	// set the cursor to column 0, line 1
	// (note: line 1 is the second row, since counting begins with 0):
	lcd.setCursor(0, 1);
	lcd.print("H");
	lcd.print(HumidityRA[index]);
	lcd.setCursor(7,1);
	lcd.print("avg=");
	lcd.print(HumidityRA[Ave]);




 /*   float temp = htu.readTemperature();
    float rel_hum = htu.readHumidity();
	Serial.print("Temp: "); Serial.print(temp); Serial.print(" C");
    Serial.print("\t\t");
 
	Serial.print("Humidity: "); Serial.print(rel_hum); Serial.println(" \%");
*/
	delay(5000);
}