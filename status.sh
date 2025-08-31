#!/bin/sh


URL_NOTIFICATION=http://172.17.123.169:8080/status
#==============================================================================
# Consumo de servicio local
#==============================================================================
notifyStatus() {
  VARDT=`date +%d\-%m\-%Y\ %H:%M:%S`
  JSON='{"message":"'$1'","date":"'${VARDT}'","cronOk":'$2'}'
  echo "DATA SEND: $JSON "
  curl -m 10 --connect-timeout 10 -d "$JSON" -H "Content-Type: application/json" -X POST $URL_NOTIFICATION | jq . > curlResult.txt
  RESPUESTA=`tail -3 curlResult.txt`
  ESPERADA="{
  \"status\": true
}" 
  if [ "$RESPUESTA" != "$ESPERADA" ]
  then
    echo "ERROR: Notificando estado"
    echo "RESPUESTA: $RESPUESTA "
    echo "ESPERADA: $ESPERADA "
  else
    echo "OK: Notificando estado"
  fi
  rm -f curlResult.txt
}


echo "  Iniciando comprobación..."
# se notifica el estado
notifyStatus "Cron de status" true
exit 0
