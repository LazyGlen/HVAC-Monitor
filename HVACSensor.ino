#include <Wire.h>
#include "Adafruit_HTU21DF.h"
#include <LiquidCrystal.h>
#include "RTClib.h"

// Connect Vin to 3-5VDC
// Connect GND to ground
// Connect SCL to I2C clock pin (A5 on UNO)
// Connect SDA to I2C data pin (A4 on UNO)

Adafruit_HTU21DF htu = Adafruit_HTU21DF();
RTC_DS1307 rtc;

	char daysOfTheWeek[7][12] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
	const int Max = 10;
	const int Ave = Max - 1;
	const int Min = Max - 2;
	const int LENGTH = 8;		// length of the circular queue to store readings for averages. Eight makes an easy number to divide by
	float TempRA[Max + 1]; 
	float HumidityRA[Max + 1];
	int index = 0;
	const int rs = 7, en = 8, d4 = 9, d5 = 10, d6 = 11, d7 = 12;
	LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
	byte DEGREE[] = {12,18,18,12,0,0,0,0};
	byte PERCENT[] = {25,26,2,4,4,8,11,19};
	DateTime now;
	DateTime when[5];

void serial_output_time(DateTime stamp, int){
	Serial.print(stamp.month(), DEC);
	Serial.print('/');
	Serial.print(stamp.day(), DEC);
	Serial.print('/');
	Serial.print(stamp.year(), DEC);
	Serial.print(" (");
	Serial.print(daysOfTheWeek[stamp.dayOfTheWeek()]);
	Serial.print(") ");
	Serial.print(stamp.hour(), DEC);
	Serial.print(':');
	Serial.print(stamp.minute(), DEC);
	Serial.print(':');
	Serial.print(stamp.second(), DEC);
	Serial.println();
/*
	Serial.println(index);
	Serial.print("Recent Temp: "); Serial.print(TempRA[index]); Serial.print(" C");
	Serial.print("\t");
	Serial.print("Min Temp: "); Serial.print(TempRA[Min]); Serial.print(" C");
	Serial.print("\t");
	Serial.print("Average Temp: "); Serial.print(TempRA[Ave]); Serial.print(" C");
	Serial.print("\t");
	Serial.print("Max Temp: "); Serial.print(TempRA[Max]); Serial.print(" C");
	Serial.println("");
	Serial.print("Recent Humidity: "); Serial.print(HumidityRA[index]); Serial.print(" C");
	Serial.print("\t");
	Serial.print("Min Humidity: "); Serial.print(HumidityRA[Min]); Serial.print(" C");
	Serial.print("\t");
	Serial.print("Average Humidity: "); Serial.print(HumidityRA[Ave]); Serial.print(" C");
	Serial.print("\t");
	Serial.print("Max Humidity: "); Serial.print(HumidityRA[Max]); Serial.print(" C");
	Serial.println("");
*/
}
	
/*
<!DOCTYPE html>
<html>
<head>
<style>
table {
  font-family: arial, sans-serif;
  border-collapse: collapse;
  width: 100%;
}

td, th {
  border: 1px solid #dddddd;
  text-align: left;
  padding: 8px;
}

tr:nth-child(even) {
  background-color: #dddddd;
}
</style>
</head>
<body>

<h2>Environmental Data</h2>

<table>
  <tr>
    <th>Value</th>
    <th>Temp</th>
    <th>Humidity</th>
    <th>Time</th>
  </tr>
  <tr>
    <td>Current</td>
    <td>Temp[index]</td>
    <td>Humidity[index]</td>
    <td>Time[index]</td>
  </tr>
  <tr>
    <td>Average</td>
    <td>Temp[Ave]</td>
    <td>Humidity[Ave]</td>
    <td>(Time[index]-Time[(index + length -1) % length])</td>
  </tr>
  <tr>
    <td>Maximum</td>
    <td>Temp[Max]</td>
    <td>Humidity[Max]</td>
    <td>Time[Max]</td>
  </tr>
  <tr>
    <td>Minimum</td>
    <td>Temp[Min]</td>
    <td>Humidity[Min]</td>
    <td>Time[Min]</td>
  </tr>
  <tr>
    <td>Previous 1</td>
    <td>Temp[(index + length -1) % length]</td>
    <td>Humidity[(index + length -1) % length]</td>
    <td>Time[(index + length -1) % length]</td>
  </tr>
  <tr>
    <td>Previous 2</td>
    <td>Temp[(index + length -2) % length]</td>
    <td>Humidity[(index + length -2) % length]</td>
    <td>Time[(index + length -2) % length]</td>
  </tr>
</table>

</body>
</html>

*/	
	
void TakeReading(){
	float AvgTemp = 0;		//allocate and initialize temporary variables to calculate the averages
	float AvgHumidity = 0;

//	if (index++ == 7)	{	// Rolling pointer to the current location in the circular queue
//		index = 0;			// moved to the top of the function, first reading on boot stored in the second queue location - don't care.
//	}
    index = (index + 1) % LENGTH;		// advance index to the next location in the circular queue

	TempRA[index] = htu.readTemperature();
	HumidityRA[index] = htu.readHumidity();
	//series of conditional assignments in case of new maximum or minimum values, also prints an alert on the serial port if found.
	if (TempRA[index] < TempRA[Min]) {TempRA[Min] = TempRA[index]; Serial.println("New Min temp!"); when[1] = now; serial_output_time(when[1], 1);}
	if (TempRA[index] > TempRA[Max]) {TempRA[Max] = TempRA[index]; Serial.println("New Max temp!"); when[2] = now; serial_output_time(when[2], 1);}
	if (HumidityRA[index] < HumidityRA[Min]) {HumidityRA[Min] = HumidityRA[index]; Serial.println("New Min Humidity!"); when[3] = now; serial_output_time(when[3], 1);}
	if (HumidityRA[index] > HumidityRA[Max]) {HumidityRA[Max] = HumidityRA[index]; Serial.println("New Max Humidity!"); when[4] = now; serial_output_time(when[4], 1);}

	// Sum the 8 most recent readings of each sensor (8 to make floating point math easy)
	for (int x = 0; x <=7; x++){
		AvgTemp += TempRA[x];
		AvgHumidity += HumidityRA[x];
	}
	//Divide the sum by the number of readings and store it in the appropriate location of the array.
	TempRA[Ave] = (AvgTemp / LENGTH);
	HumidityRA[Ave] = (AvgHumidity / LENGTH);
}


void LCD_output(float Data[], int size, int symbol){
	// set the cursor to column 0, line 0
	lcd.setCursor(0, 0);
	lcd.print(Data[index]);
	lcd.write(byte(symbol));			// when calling lcd.write() the symbol must be cast as a byte
	lcd.setCursor(10,0);
	lcd.print(Data[Max]);
	lcd.write(byte(symbol));			// when calling lcd.write() the symbol must be cast as a byte
	// set the cursor to column 0, line 1
	// (note: line 1 is the second row, since counting begins with 0):
	lcd.setCursor(0, 1);
	lcd.print(Data[Ave]);
	lcd.write(byte(symbol));			// when calling lcd.write() the symbol must be cast as a byte
	lcd.setCursor(10,1);
	lcd.print(Data[Min]);
	lcd.write(byte(symbol));			// when calling lcd.write() the symbol must be cast as a byte

}


void setup() {
	Serial.begin(9600);
	Serial.println("HTU21D-F test");

	if (!htu.begin()) {
		Serial.println("Couldn't find sensor!");
		while (1);
	}
	
	//  initialize the minimums so it isn't zero.
	now = rtc.now();
  	TempRA[Min] = htu.readTemperature();
	when[1] = now;
	HumidityRA[Min] = htu.readHumidity();
	when[3] = now;

	// set up the LCD's number of columns and rows:
	lcd.begin(16, 2);
	// Print a message to the LCD.
	lcd.print("Hello, Glen!");
  	lcd.createChar(0, DEGREE);
  	lcd.createChar(1, PERCENT);

	
	if (! rtc.begin()) {
		Serial.println("Couldn't find RTC");
	while (1);
	}
	if (! rtc.isrunning()) {
		Serial.println("RTC is NOT running!");
		// following line sets the RTC to the date & time this sketch was compiled
		// rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
		// This line sets the RTC with an explicit date & time, for example to set
		// January 21, 2014 at 3am you would call:
		// rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
	}

	delay(1000);
	lcd.clear();

}

void loop() {
	TakeReading();
	now = rtc.now();

	serial_output_time(now, 1);
	Serial.print("minimum temperature ");
	Serial.print(TempRA[Min]);
	Serial.print(" sampled at: ");
	serial_output_time(when[1], 1);
	Serial.print("maximum temperature sampled at: ");
	serial_output_time(when[2], 1);
	Serial.print("minimum humidity sampled at: ");
	serial_output_time(when[3], 1);
	Serial.print("maximum humidity sampled at: ");
	serial_output_time(when[4], 1);

	
	LCD_output( TempRA, (Max + 1) , 0);
	delay(3000); // Leave temperatures on LCD for 3 seconds
	lcd.clear();
	
	LCD_output( HumidityRA, (Max + 1) , 1);
	delay(3000); // Leave Humidities on LCD for 3 seconds
	lcd.clear();
	
	lcd.setCursor(1, 0);
	lcd.print(daysOfTheWeek[now.dayOfTheWeek()]);
	lcd.print(" ");
	lcd.print(now.month(), DEC);
	lcd.print('/');
	lcd.print(now.day(), DEC);
	lcd.print('/');
	lcd.print(now.year(), DEC);
	lcd.setCursor(4, 1);
	lcd.print(now.hour(), DEC);
	lcd.print(':');
	lcd.print(now.minute(), DEC);
	lcd.print(':');
	lcd.print(now.second(), DEC);
	delay(2000);
	lcd.clear();
	

}