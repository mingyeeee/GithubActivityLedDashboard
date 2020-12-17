# GithubActivityLedDashboard
An LED dashboard to show your Github commit activity. I made this as a physical reminder to work on my coding projects. Seeing my (lack of) activity on my github is a great motivational tool.  
The dashboard shows 2 weeks worth of Github activity using LEDs to indicate activity. 

## Hardware
An ESP32 is used to get the github activity information from a ThinkSpeak API. It also controls the LEDs for the dashboard.
WS2811 LED strips are used for indicating github activity.

Pin 27 -> ledstrip data line

## ThinkSpeak API setup
To get the github commit activity, ThingSpeak API is used to webscrape the data from the github profile using an xpath. 

* URL: 	https://github.com/users/YOUR_USERNAME/contributions?to=CURRENT_YEAR

Parse String (Xpath): 	
1. /html/body/div/div/div/div[1]/svg/g/g[52]
2. /html/body/div/div/div/div[1]/svg/g/g[53]

![ThinkSpeakAPIexample](/images/thingspeakApiExample.png)
