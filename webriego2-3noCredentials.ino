
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Firebase_ESP_Client.h>

//Provide the token generation process info.
#include <addons/TokenHelper.h>

//Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>
#define API_KEY "AIzaSyC5Hy6iLwsqmJzJJ9cfOt1an9EHINURQbs"

/* 3. Define the user Email and password that already registerd or added in your project */
#define USER_EMAIL "cliniobora@gmail.com"
#define USER_PASSWORD "PERONPERON1949"

/* 4. If work with RTDB, define the RTDB URL */
#define DATABASE_URL "primeriot-72217-default-rtdb.firebaseio.com/" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app

#define DATABASE_SECRET "DATABASE_SECRET"

/* 6. Define the Firebase Data object */
FirebaseData fbdo;

/* 7. Define the FirebaseAuth data for authentication data */
FirebaseAuth auth;

/* 8. Define the FirebaseConfig data for config data */
FirebaseConfig config;

unsigned long dataMillis = 0;
int cnt = 0;


#define WIFI_SSID "Fibertel WiFi418 2.4GHz" // your wifi SSID
#define WIFI_PASSWORD "0143245003" //your wifi PASSWORD
#define PROJECT_ID "primeriot-72217"

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "ar.pool.ntp.org", -10800);
////////////////////
byte pinLed = 16;
int horas = 0;
volatile int segundos = 0;
volatile boolean actualizartime = true;
int minutos;
boolean actualizar = true;//elemento del reloj logico

int estadoRele = 0;//dato que se envia al relay

float RxD = 1 ; //Var Riegos por dia, cantidad de veces que riega en el dia. Default=1
byte HR = 0; //Almaceno la Hora real
byte HA = 8;  ; //Hora Ancla, Comienza a regar siempre a la misma hora (real), Default=8am
byte HV = 0; //Hora Volatil, Vuelve a 0 cada 24hs/RxD
byte FH = RxD * 24; //Tope de HV
boolean anclar = false;

int horadeinicio = 0;
int minutodeinicio = 0;


int tiemporestante; //cuando es mayor a 0 esta regando,
//se resta por segundos
int contadordesegundos; //pausa logica para el riego automatico
int contadordehoras; //pausa logica para el riego automatico

boolean cooldownRiego = true;

int tiempoderiego = 0; //periodo de tiempo elegido
//por el usuario representa el tiempo
//de duracion del riego ya sea
//manual o automatico
boolean desactivado = false;// desactiva el riego automatico

int HoradeRiego;
int OnOff;
int RiegoCada;
int RiegoCadaOperable = 24;
bool flagRCO = true;
int RiegoDurante;
int RiegoManual;
///////////////////////

void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(pinLed, OUTPUT);
  digitalWrite(pinLed, 1);


  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  timeClient.begin();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL */
  config.database_url = DATABASE_URL;

  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(2048);

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  /** Assign the maximum retry of token generation */
  config.max_token_generation_retry = 5;

  /* Initialize the library with the Firebase authen and config */
  Firebase.begin(&config, &auth);



}

void rele() {

  if (tiemporestante > 0) {
    if (segundos != contadordesegundos) {
      contadordesegundos = segundos;
      tiemporestante--;
      estadoRele = 0;
    }
  }
  else {
    estadoRele = 1;
  }
  digitalWrite(pinLed, estadoRele);
}
void Sintesis() {

  if (RiegoCadaOperable == 0) {   //El RiegoCadaOperable es un checkpoint desde el cual se habilita
    //el riego. Se reinicia en dos casos: Cuando se cumple la Hora de Riego Fija y cuando se cumplen las horas configuradas para riego
    tiemporestante = RiegoDurante;
    RiegoCadaOperable = RiegoCada;

  }
  if (horas != contadordehoras) { //CUANDO PASE UNA HORA...
    contadordehoras = horas;
    RiegoCadaOperable--;

  }

  ///////////////////////////////////////////////////////////
  else if (horas == HoradeRiego && cooldownRiego == true) {
    if (RiegoCada < 25) {
      RiegoCadaOperable = 0;
      cooldownRiego = false;
    }

  }
  else if (horas != HoradeRiego) {
    cooldownRiego = true;
  }

  ////////////////////
}

void actualizarReloj() {

  timeClient.update();
  horas = timeClient.getHours();
  minutos = timeClient.getMinutes();
  segundos = timeClient.getSeconds();

}
void loop() {
  actualizarReloj();
  LeerDB();
  if (OnOff) {
    Sintesis();
  }
  else {
    tiemporestante = 0;
  }
  rele();
}

void LeerDB() {

  if (millis() - dataMillis > 35000 && Firebase.ready())
  {
    dataMillis = millis();


    //int fbHoradeRiego = Firebase.RTDB.getInt(&fbdo, "HoradeRiego");
    //Firebase.RTDB.getInt(&fbdo, F("HoradeRiego"), &iVal) ? String(iVal).c_str() : fbdo.errorReason().c_str();
    //Firebase.RTDB.getInt(&fbdo, "HoradeRiego");
    Firebase.RTDB.getInt(&fbdo, "HoradeRiego", &HoradeRiego);
    Firebase.RTDB.getInt(&fbdo, "OnOff", &OnOff);
    Firebase.RTDB.getInt(&fbdo, "RiegoCada", &RiegoCada);
    if (flagRCO) {
      RiegoCadaOperable = RiegoCada;
      flagRCO = false;
      contadordehoras=horas;
    }

    Firebase.RTDB.getInt(&fbdo, "RiegoDurante", &RiegoDurante);

    Firebase.RTDB.getInt(&fbdo, "RiegoManual", &RiegoManual);
    if (RiegoManual == 1) {
      tiemporestante = RiegoDurante;
      Firebase.RTDB.setInt(&fbdo, "RiegoManual", 0);
    }

    /*int fbOnOff =  firebase.getInt("OnOff"); //LECTURA(objetofdb,"nodo")
      int fbRiegoCada = firebase.getInt("RiegoCada"); //LECTURA(objetofdb,"nodo")
      int fbRiegoDurante = firebase.getInt("RiegoDurante"); //LECTURA(objetofdb,"nodo")
      int fbRiegoManual =   firebase.getInt("RiegoManual"); //LECTURA(objetofdb,"nodo")
      Serial.println("OnOf:" + fbOnOff);
      Serial.println("HoradeRiego:" + fbHoradeRiego);
      Serial.println("RiegoCada:" + fbRiegoCada);
      Serial.println("RiegoDurante:" + fbRiegoDurante);
      Serial.println("RiegoManual:" + fbRiegoManual);
      Serial.println(" HoradeRiego:" + fbHoradeRiego);
      Serial.println(" HoradeRiego1:" + iVal);


      Serial.println(horas);
      Serial.println(minutos);
      Serial.println(segundos);
      Serial.println(HoradeRiego);
      Serial.println(OnOff);
      Serial.println(RiegoCada);
      Serial.println(RiegoDurante);
      Serial.println(RiegoManual);*/


  }

  /*Firebase.setInt(firebaseData,"HoradeRiego",);//LECTURA(objetofdb,"nodo",valor)
      delay(500);

  */

}
