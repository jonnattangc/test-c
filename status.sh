#!/bin/sh


URL_NOTIFICATION=http://10.0.0.233:8080/status
#==============================================================================
# Consumo de servicio local
#==============================================================================
notifyStatus() {
  VARDT=`date +%d\-%m\-%Y\ %H:%M:%S`
  JSON='{"message":"'$1'","date":"'${VARDT}'","cronOk":'$2'}'
  echo "DATA SEND: $JSON "
  curl -m 10 --connect-timeout 10 -d "$JSON" -H "Content-Type: application/json" -X POST $URL_NOTIFICATION | jq . > curlResult.txt
  RESPUESTA=`tail -3 curlResult.txt`
  # echo "RESPUESTA: $RESPUESTA "
  ESPERADA="{
  \"status\": true
}"
  # echo "ESPERADA: $ESPERADA "
  if [ "$RESPUESTA" == "$ESPERADA" ]; then
    rm -f curlResult.txt
  else
    echo "ERROR: Notificando estado"
  fi
}


echo "  Iniciando comprobación..."
# se notifica el estado
notifyStatus "Cron de status" true
exit 0
