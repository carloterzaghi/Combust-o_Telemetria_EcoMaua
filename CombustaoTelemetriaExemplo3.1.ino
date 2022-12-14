
/* Constantes e variaveis para definir o tempo entre iterações */

const uint32_t culiExecTimingsMsValue = 1000;

uint32_t uliCurrTimeMs  = 0;
uint32_t uliLastTimesMs = 0;

/* Constantes e variaveis para criar a data, velocidade e posição de teste */

uint8_t ucDatetimeHour   = 12;
uint8_t ucDatetimeMinute = 0;
uint8_t ucDatetimeSecond = 0;

char cDatetimeStr[9];

float fVehicleSpeedKMpH  = 0.0f;
float fVehicleSpeedMpS   = 0.0f;
float fVehicleDistanceM  = 0.0f;
float fVehicleDistanceKM = 0.0f;

float fVehicleLastSpeedMpS = 0.0f;

const float cfIterationTimeS = 1.0f;
const float cfMaxSpeedKMpH   = 30.0f;
const float cfSpeedAccelRate = cfMaxSpeedKMpH / 500.0f;
const float cfSpeedBrakeRate = cfMaxSpeedKMpH / 500.0f;
int botao;
int estado;

uint16_t usiSpeedCnt = 1;

String InBytes;

void setup() {
  pinMode(10, OUTPUT);
  // As coisas que estão em comentário a maioria é para o botão
//  pinMode(8, INPUT_PULLUP);
  
  /* Inicia o Serial */
  Serial.begin(9600);

  /* Espera o serial iniciar */
  delay(1000);

  /* Envia o header dos dados */
  Serial.print("tempo_hh:mm:ss");
  Serial.print(",");
  Serial.print("velocidade_kmh");
  Serial.print(",");
  Serial.println("distancia_km");
//  Serial.print(",");
//  Serial.println("botao_1S_0N");
  
}

// the loop routine runs over and over again forever:
void loop() {
//  estado = digitalRead(8);
//  switch (estado){
//    case HIGH:
//      botao = 0;
//      break;
//
//    case LOW:
//      botao = 1;
//      break;
//  }

if (Serial.available()>0){
  InBytes = Serial.readStringUntil('\n');
  if (InBytes == 'on'){
    Serial.write('Ficou on');
    digitalWrite(10, HIGH);    
  }
  else{
    Serial.write('Ficou off');
    digitalWrite(10, LOW);    
  }
}

  /* Atualiza o tempo atual, em ms */
  uliCurrTimeMs = millis();

  /* Verifica se o tempo de execução passou desde da última ocorrência */
  /* Aqui, o type casting para um tipo unsigned com o mesmo tamanho do temporizados é importante para computar corretamente a diferença de tempo, mesmo que um overflow ocorra */
  if (culiExecTimingsMsValue < (uint32_t) (uliCurrTimeMs - uliLastTimesMs)) {
    uliLastTimesMs = uliCurrTimeMs;
    
    /* O tempo de execução passou, executar código temporizado */

    /* Calcula a velocidade atual */
    if (usiSpeedCnt < 200) {
      fVehicleSpeedKMpH = 0.0f;
    } else if (usiSpeedCnt < 700) {
      fVehicleSpeedKMpH += cfSpeedAccelRate;
    } else if (usiSpeedCnt < 1100) {
      fVehicleSpeedKMpH = cfMaxSpeedKMpH;
    } else if (usiSpeedCnt < 1600) {
      fVehicleSpeedKMpH -= cfSpeedAccelRate;
    } else if (usiSpeedCnt < 1800) {
      fVehicleSpeedKMpH = 0.0f;
    } else {
      usiSpeedCnt = 0;
    }
    usiSpeedCnt++;

    /* Satura a velocidade atual */
    if (fVehicleSpeedKMpH < 0.0f){
      fVehicleSpeedKMpH = 0.0f;
    } else if (fVehicleSpeedKMpH > cfMaxSpeedKMpH){
      fVehicleSpeedKMpH = cfMaxSpeedKMpH;
    }

    /* Atualiza a última velocidade e converte a atual de km/h para m/s para integrar usando segundos e obter metros */
    fVehicleLastSpeedMpS = fVehicleSpeedMpS;
    fVehicleSpeedMpS = fVehicleSpeedKMpH / 3.6f;

    /* Integra a velocidade para obter a distancia percorrida */
    fVehicleDistanceM += cfIterationTimeS * 0.5f * (fVehicleLastSpeedMpS + fVehicleSpeedMpS);

    /* Converte a distancia de m para km */
    fVehicleDistanceKM = fVehicleDistanceM / 1000.0f;

    /* Transforma o tempo atual em uma string formatada */
    sprintf(cDatetimeStr, "%02u:%02u:%02u", ucDatetimeHour, ucDatetimeMinute, ucDatetimeSecond);

    /* Envia as informações de telemetria por serial no formato CVS (comma-separated values) */
    Serial.print(cDatetimeStr);
    Serial.print(",");
    Serial.print(fVehicleSpeedKMpH, 6);
    Serial.print(",");
    Serial.println(fVehicleDistanceKM, 6);
//    Serial.print(",");
//    Serial.println(botao);
    
    /* Atualiza a data */
    ucDatetimeSecond++;
    if (ucDatetimeSecond >= 60) {
      ucDatetimeSecond = 0;
      ucDatetimeMinute++;
      if (ucDatetimeMinute >= 60) {
        ucDatetimeMinute = 0;
        ucDatetimeHour++;
        if (ucDatetimeHour >= 24) {
          ucDatetimeHour = 0;
        }
      }
    }

  } /* Fim da execução temporizada */

}
