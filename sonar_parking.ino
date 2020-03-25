
/*
Information obtained:
16 bits total not including start 
bits 14-16   always  111
bit 13    0 for reverse 1 for not reverse
bit 12    0 for valid data present 
bits 9-11 ( + bit 13 )
    sensor A  0
    sensor B  8
    sensor C  4
    sensor D  12
    sensor E  1
    sensor F  9
    sensor G  5
    sensor H  13
    
Blue connected :  EFGH  ( 4 Front )
Green and Green + Blue connected :  ABCD EH  ( 2 Front + 4 Rear )


*/

int pin = 7;
int greenPin = 4;
int bluePin = 5;
int dataOffset = 0;


#define DATA_SIZE 17
unsigned long pulse_length;
byte distances[8]; 
byte pulse_value[DATA_SIZE];          // holds the binary equivalents
byte temp = 0;
bool datachange = true;

void setup()
{
pinMode(pin, INPUT);
pinMode(greenPin, OUTPUT);
pinMode(bluePin, OUTPUT);

digitalWrite(greenPin, HIGH);

Serial.begin(115200);
Serial.println("STARTED");
}

void setDistance(byte sensorIdx,byte newdistance)
{
  if ( distances[sensorIdx] != newdistance )
  {
    datachange = true;
    distances[sensorIdx] = newdistance ;
  }
}

void sendSensors()
{
  Serial.print("{");
  for( int i=0; i <8; i++)
  {
    if (i>0)
      Serial.print(";");
    Serial.print("\"");
    Serial.print(char(i+'A'));
    Serial.print("\":");
    Serial.print(distances[i]);
    
  }


  Serial.println("} ");
  datachange = false;
}

void processCMD()
{
//print as sensor id and distance -- valid data only

if ( (pulse_value[0] == 1 )&(pulse_value[14] == 0 )&(pulse_value[15] == 0 )&(pulse_value[16] == 0 ) )   //data valid
  {

    if ( pulse_value[12] == 1) // distance info valid 
    {

    byte sensor_id = 0;   //will hold sensor id bin
    if (pulse_value[13]==1) bitSet(sensor_id,0);
    if (pulse_value[11]==1) bitSet(sensor_id,1);
    if (pulse_value[10]==1) bitSet(sensor_id,2);
    if (pulse_value[9]==1) bitSet(sensor_id,3);
    byte sensor_dist_c = 0;  //will hold sensor distance
    if (pulse_value[8]==1) bitSet(sensor_dist_c,7);
    if (pulse_value[7]==1) bitSet(sensor_dist_c,6);
    if (pulse_value[6]==1) bitSet(sensor_dist_c,5);
    if (pulse_value[5]==1) bitSet(sensor_dist_c,4);
    if (pulse_value[4]==1) bitSet(sensor_dist_c,3);
    if (pulse_value[3]==1) bitSet(sensor_dist_c,2);
    if (pulse_value[2]==1) bitSet(sensor_dist_c,1);
    if (pulse_value[1]==1) bitSet(sensor_dist_c,0);

    if (sensor_id == 0) 
      setDistance(0,sensor_dist_c);
    if (sensor_id == 8)  
      setDistance(1,sensor_dist_c);
    if (sensor_id == 4)  
      setDistance(2,sensor_dist_c);
    if (sensor_id == 12) 
      setDistance(3,sensor_dist_c);
    if (sensor_id == 1)  
      setDistance(4,sensor_dist_c);
    if (sensor_id == 9)  
      setDistance(5,sensor_dist_c);
    if (sensor_id == 5)  
      setDistance(6,sensor_dist_c);
    if (sensor_id == 13)  
      setDistance(7,sensor_dist_c);
  
    }
    
    else
    {
      // reset sensor values
      for( int i=0; i <8; i++)
      {
        if ( distances[i] != 0)
          datachange = true;
        distances[i]=0;
        
      }
      
    }
  
  } 
else
  {
    Serial.print("INVALID DATA : ");
    for( int i=0; i <DATA_SIZE; i++)
      Serial.print(pulse_value[i]);
    Serial.println(".");
  }
}

void readSensors()
  {
    if (pulse_length > 800) {           //start pulse
      dataOffset=0;
   // Serial.println(".");
      pulse_value[dataOffset] = 1;
    }
    else if (pulse_length < 300) 
      pulse_value[dataOffset] = 1;
    else 
      pulse_value[dataOffset] = 0;
  
    // store pulse length
   // pulse_set[dataOffset]=pulse_length;
    // Debug pulse
   // if (pulse_length != 0) 
   //   Serial.print(pulse_value[dataOffset]);
  
    dataOffset++;
    // Command Completed 
    if (dataOffset == DATA_SIZE )
    {
      dataOffset = 0; 
      processCMD();
    }
  }

void loop(){
  
  pulse_length = pulseIn(pin, HIGH);

  if (pulse_length != 0) // Data Received
    readSensors();

  if (datachange)
    sendSensors();

}
