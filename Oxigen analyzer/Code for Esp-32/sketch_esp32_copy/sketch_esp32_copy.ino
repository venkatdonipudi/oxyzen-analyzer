#include <Adafruit_ADS1X15.h> 

Adafruit_ADS1115 ads; 

// Macros
#define OOM202 0

// Global Variables
int16_t adc_OOM202;
float mV_OOM202;
float current_O2Percent = 0.00;

float o2Con_knownValue = 93;
float mV_baselineO2 = 15.4;   // millivolt value for 20.9% Ambient Air 

const float ambientAir_O2Percent = 20.9;//oxigen persentage present in earth surface.

void setup(void) 
{
  Serial.begin(115200);
  Serial.println("----------------------- Oxygen Analyzer -----------------------");

  ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit =  0.0078125mV
  Serial.println("ADC Gain Set to16X (+/- 0.256V, 1-Bit = 0.0078125 mV");

  ads.begin();    // Initializing the ADS1115
  Serial.println("ADS1115 Initialized");

  Serial.println("For Calibration type <cal> and click send button");
}

void loop()
{
  char calMode[4] = {'e','x','t','\0'};
  char count = 0;
  
  while(Serial.available())
  {
    calMode[count] = Serial.read();
    count++;
  }

  if(calMode == "cal")
  {
    calibrate_OOM202();
    calMode[0] = 'e';
  }

  readOOM202();     // Calling this function to read OOM202 oxygen sensor. 

  delay(2000);    // Wait for 2 Sec
}

/*
 * Read OOM202 Oxygen Sensor connected to A0 of ADS1115
 */
void readOOM202()
{
  while(1){
  adc_OOM202 = ads.readADC_SingleEnded(OOM202);   // Read the ADC Value of OOM202 Sensor
  mV_OOM202 = ads.computeVolts(adc_OOM202);       // Compute the voltage Input from OOM202 Sensor
  mV_OOM202 = mV_OOM202 * 1000;                   // Convert volatge computed to millivolt (mV)

  current_O2Percent = ((mV_OOM202 / mV_baselineO2) * ambientAir_O2Percent); // Convert analog voltage to percentage

  Serial.print("Analog Value: "); Serial.print(mV_OOM202); Serial.print(" mV"); 
  Serial.print("    ");
  Serial.print("O2 Percentage: "); Serial.print(current_O2Percent); Serial.println(" %");
  delay(300);
  }
}

/*
 * Calibrate your OOM202 Sensor
 */

void calibrate_OOM202()
{
  char calProcess = 1;
  int count = 1;
  float mV_avgValue = 0.00, mV_SampleO2 = 0.00;
  float error = 0.00, baseline = 0.00;
  
  while(calProcess)
  {
    Serial.println("Enter the % of O2 using for Calibration");

    while(count)
    {
      if(Serial.available())
      {
        o2Con_knownValue = Serial.read(); 
        count = 0;
      }
    }

    Serial.println("Place the Sensor to Known O2 % for 20 Secs");
    Serial.println("20 Secs Starts now.......");
    delay(5000);
    for(count = 0;count<20;count++)
    {
      readOOM202();
      mV_avgValue += mV_OOM202;
      delay(100);
    }
    mV_avgValue = mV_avgValue / 20;
    mV_SampleO2 = mV_avgValue;

    Serial.print("Sensor millivolt value for ");
    Serial.print(o2Con_knownValue);
    Serial.print(" % = ");
    Serial.print(mV_SampleO2);
    Serial.println(" mV");

    baseline = ((mV_SampleO2 * ambientAir_O2Percent) / o2Con_knownValue);

    if(mV_baselineO2 > baseline)
      error = mV_baselineO2 - baseline;
    if(baseline > mV_baselineO2)
      error = baseline - mV_baselineO2;

    mV_baselineO2 -= error;

    Serial.println("Calibration Done");
    calProcess = 0;
    
  }
}
  