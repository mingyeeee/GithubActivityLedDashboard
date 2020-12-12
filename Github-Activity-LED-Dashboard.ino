/* Mingye Chen 2020-12-12
 * To do: Add LED strip feature
 * Gets Github commit activity data and displays it on an LED Dashboard
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "credentials.h"

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
int hour, minutes;

//WIFI information from credentials.h
const char* ssid = mySSID;
const char* password =  myPASSWORD;
//api urls for last week and the current week from credentials.h
const char * thisweek = GITHUB_PROFILE_LINK_1;
const char * lastweek = GITHUB_PROFILE_LINK_2;
int githubActivity[14];


void getGithubActivity(const char * url){
  //github commit activity data for a week
  int activityData[7] = {0,0,0,0,0,0,0};
  HTTPClient http;
 
    http.begin(url);
    // HTTP request
    int httpCode = http.GET();
    // If API response is successful
    if (httpCode > 0) {
        // Get text body of the HTTP response
        String payload = http.getString();
        int payloadLength = payload.length();
        char response[payload.length()];
        // Convert String to Char array to avoid Arduino's String() memory issues
        payload.toCharArray(response, payloadLength);
        Serial.println(httpCode);
        Serial.println(response);
        /* SAMPLE RESPONSE - data-count="x" is the data we use
        <rect class="day" width="11" height="11" x="-35" y="0" fill="var(--color-calendar-graph-day-bg)" data-count="0" data-date="2020-11-29"></rect>
        <rect class="day" width="11" height="11" x="-35" y="15" fill="var(--color-calendar-graph-day-bg)" data-count="0" data-date="2020-11-30"></rect>
        <rect class="day" width="11" height="11" x="-35" y="30" fill="var(--color-calendar-graph-day-bg)" data-count="0" data-date="2020-12-01"></rect>
        <rect class="day" width="11" height="11" x="-35" y="45" fill="var(--color-calendar-graph-day-L3-bg)" data-count="5" data-date="2020-12-02"></rect>
        <rect class="day" width="11" height="11" x="-35" y="60" fill="var(--color-calendar-graph-day-L2-bg)" data-count="4" data-date="2020-12-03"></rect>
        <rect class="day" width="11" height="11" x="-35" y="75" fill="var(--color-calendar-graph-day-bg)" data-count="0" data-date="2020-12-04"></rect>
        <rect class="day" width="11" height="11" x="-35" y="90" fill="var(--color-calendar-graph-day-bg)" data-count="0" data-date="2020-12-05"></rect>
       */
        
        // Get size of the message
        size_t responseSize = sizeof(response)/sizeof(char);
        Serial.println(responseSize);
        // Each row as about 155 characters. This is used to efficently manipulate the string to get the data.
        int rows = responseSize/155;
        int counter = 0;
        // search row by row
        for(int i = 0; i < rows; i++){
          for(int j = 155*i; j<(155*(i+1))-15; j++){
            // if "nt" (last characters of "count") is found then the index for the github activity data is nearby
            if(response[j] == 'n' && response[j+1] == 't'){
              // skip to the number which is the data we need
              j += 4;
              
              // Debugging
              //Serial.print("data: "); Serial.println(response[j]);
              
              // If data is only 1 digit
              if(response[j+1] == '\"'){
                // char to int
                activityData[i] = response[j] - '0';
                Serial.println(activityData[i]); 
              }
              // If data is 2 digits
              else{
                // char to int
                activityData[i] = (response[j] - '0')*10 + response[j+1] - '0';
                Serial.println(activityData[i]);
              }
              // Keep track of how many data values we have for debugging purposes
              counter++;
            }
          }
        }
        // Debugging
        //Serial.print("counter: "); Serial.println(counter);
    }
    // Error handling
    else {
      Serial.println("Error on HTTP request");
    }
 
    http.end(); //Free the resources
    // Store results in order of date in an array
    // Last week's activity
    if(url == lastweek){
      for(int i = 0; i < 7; i++){
        githubActivity[i] = activityData[i];
      }
    }
    // This Week's activity
    else {
      for(int i = 7; i < 14; i++){
        githubActivity[i] = activityData[i-7];
      }
    }
}

void setup() {
  
  Serial.begin(115200);
  delay(4000);
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
 
  Serial.println("Connected to the WiFi network");

  timeClient.begin();
  // GMT to EST 5hour offset
  timeClient.setTimeOffset(-18000);
  hour = timeClient.getHours();
}
 
void loop() {
 
  if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
    timeClient.update();
    // Checks if an hour has passed
    if(hour != timeClient.getHours()){
      hour = timeClient.getHours();
      getGithubActivity(thisweek);
      getGithubActivity(lastweek);
      for(int i = 0; i < 14; i++){
        Serial.print("data: "); Serial.println(githubActivity[i]); 
      }
    }
    else{
      // Set a delay until the next hour
      long delayUntilNextHour = 60000*(60 - timeClient.getMinutes());
      Serial.print("Delay set for "); Serial.println(delayUntilNextHour);
      delay(delayUntilNextHour);
    }
  }
}
